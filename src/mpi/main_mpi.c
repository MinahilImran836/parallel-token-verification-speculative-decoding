#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <mpi.h>

#define BENCHMARK_REPEAT 50
static uint64_t g_rng_seed = 0x853C49E6748FEA9BULL;

static inline uint64_t xs64(uint64_t s)
{
    s ^= s << 13;
    s ^= s >> 7;
    s ^= s << 17;
    return s;
}

static inline double rng_uniform(uint64_t *rng_state)
{
    *rng_state = xs64(*rng_state);
    return (double)(*rng_state >> 11) * (1.0 / (double)(1ULL << 53));
}

static inline int accept_token(float log_p, float log_q, uint64_t *rng_state)
{
    double diff = (double)(log_p - log_q);
    if (diff > 0.0) diff = 0.0;         
    return rng_uniform(rng_state) < exp(diff);
}

typedef struct {
    int   token_id;
    float log_q;
    float log_p;
} TokenData;

typedef struct {
    int        round_id;
    TokenData *tokens;    
} Round;

typedef struct {
    int    n_rounds;
    int    B;
    int    K;
    int    prompt_len;
    int   *prompt;
    Round *rounds;
} RoundsData;

#define RD1(f,x)     (fread(&(x),  sizeof(x),        1,   (f)) == 1)
#define RDN(f,p,n,T) (fread( (p),  sizeof(T), (size_t)(n),(f)) == (size_t)(n))

static RoundsData *load_rounds(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Error: cannot open %s\n", path); return NULL; }

    RoundsData *rd = (RoundsData *)calloc(1, sizeof(RoundsData));
    if (!rd) { fprintf(stderr, "OOM\n"); fclose(f); return NULL; }

    if (!RD1(f, rd->n_rounds) || !RD1(f, rd->B) ||
        !RD1(f, rd->K)        || !RD1(f, rd->prompt_len))
        goto err;

    fprintf(stderr, "[info] rounds=%d  B=%d  K=%d  prompt_len=%d\n",
            rd->n_rounds, rd->B, rd->K, rd->prompt_len);

    rd->prompt = (int *)malloc(rd->prompt_len * sizeof(int));
    if (!rd->prompt || !RDN(f, rd->prompt, rd->prompt_len, int)) goto err;

    rd->rounds = (Round *)calloc(rd->n_rounds, sizeof(Round));
    if (!rd->rounds) goto err;

    for (int r = 0; r < rd->n_rounds; r++) {
        Round *ro = &rd->rounds[r];
        if (!RD1(f, ro->round_id)) goto err;

        int total = rd->B * rd->K;
        ro->tokens = (TokenData *)malloc(total * sizeof(TokenData));
        if (!ro->tokens) goto err;

        for (int i = 0; i < total; i++)
            if (!RD1(f, ro->tokens[i].token_id) ||
                !RD1(f, ro->tokens[i].log_q)    ||
                !RD1(f, ro->tokens[i].log_p))   goto err;
    }

    fclose(f);
    return rd;

err:
    fprintf(stderr, "Error: truncated or corrupt %s\n", path);
    fclose(f);
    return NULL;
}

static void free_rounds(RoundsData *rd)
{
    if (!rd) return;
    free(rd->prompt);
    if (rd->rounds) {
        for (int r = 0; r < rd->n_rounds; r++) free(rd->rounds[r].tokens);
        free(rd->rounds);
    }
    free(rd);
}

#define MAX_TOK_STR 64
typedef struct { int id; char text[MAX_TOK_STR]; } VocabEntry;

static VocabEntry *vocab     = NULL;
static int         vocab_len = 0;

static void load_vocab(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) return;

    int lines = 0; char buf[256];
    while (fgets(buf, sizeof(buf), f)) lines++;
    rewind(f);

    vocab = (VocabEntry *)malloc(lines * sizeof(VocabEntry));
    if (!vocab) { fclose(f); return; }

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char *tab = strchr(line, '\t');
        if (!tab) continue;
        vocab[vocab_len].id = (int)strtol(line, NULL, 10);
        char *src = tab + 1;
        int   len = (int)strlen(src);
        if (len > 0 && src[len-1] == '\n') src[--len] = '\0';
        if (len > 0 && src[len-1] == '\r') src[--len] = '\0';
        strncpy(vocab[vocab_len].text, src, MAX_TOK_STR - 1);
        vocab[vocab_len].text[MAX_TOK_STR - 1] = '\0';
        vocab_len++;
    }
    fclose(f);
}

static const char *detok(int id)
{
    if (!vocab || !vocab_len) return NULL;
    int lo = 0, hi = vocab_len - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        if      (vocab[mid].id == id) return vocab[mid].text;
        else if (vocab[mid].id  < id) lo = mid + 1;
        else                          hi = mid - 1;
    }
    return NULL;
}

static void print_tokens(const int *ids, int len)
{
    for (int i = 0; i < len; i++) {
        const char *w = detok(ids[i]);
        printf("%s", w ? w : "<?>");
    }
}

static int verify_one_candidate(const TokenData *toks, int K, double *score, uint64_t *rng_state)
{
    *score = 0.0;
    for (int k = 0; k < K; k++) {
        if (!accept_token(toks[k].log_p, toks[k].log_q, rng_state))
            return k;
        *score += (double)toks[k].log_p;
    }
    return K;
}

typedef struct {
    int             winner;          
    int             winner_len;      
    const TokenData *winner_tokens;  
    long            total_accepted;  
    long            total_rejected;  
    int             fully_accepted;  
    double          elapsed_sec;
} RoundResult;

static RoundResult verify_round_mpi(const TokenData *tokens, int B, int K, int rank, int size)
{
    RoundResult res = {0};
    double best_score = -1e18;
    
    struct timespec t0, t1;
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
    }
    
    int chunk_size = (B + size - 1) / size;
    int start_idx = rank * chunk_size;
    int end_idx = (rank + 1) * chunk_size;
    if (end_idx > B) end_idx = B;
    int local_B = end_idx - start_idx;
    
    uint64_t rng_state = g_rng_seed ^ rank;
    
    for(int repeat = 0; repeat < BENCHMARK_REPEAT; repeat++)
    {
        RoundResult tmp = {0};
        double best = -1e18;
        
        tmp.winner = -1;
        tmp.winner_len = 0;
        tmp.winner_tokens = NULL;
        
        for (int b = start_idx; b < end_idx; b++) {
            double score;
            int len = verify_one_candidate(&tokens[b * K], K, &score, &rng_state);
            
            tmp.total_accepted += len;
            tmp.total_rejected += (K - len);
            
            if (len == K)
                tmp.fully_accepted++;
            
            if (len > tmp.winner_len ||
                (len == tmp.winner_len && score > best)) {
                tmp.winner = b;
                tmp.winner_len = len;
                tmp.winner_tokens = &tokens[b * K];
                best = score;
            }
        }
        
        long local_accepted = tmp.total_accepted;
        long local_rejected = tmp.total_rejected;
        int local_fully = tmp.fully_accepted;
        int local_winner = tmp.winner;
        int local_winner_len = tmp.winner_len;
        double local_best = best;
        
        long global_accepted, global_rejected;
        int global_fully;
        
        MPI_Reduce(&local_accepted, &global_accepted, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_rejected, &global_rejected, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
        MPI_Reduce(&local_fully, &global_fully, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        
        // Manual winner reduction: gather all local winners and pick best on rank 0
        struct {
            int winner;
            int winner_len;
            double best_score;
        } local_result = {local_winner, local_winner_len, local_best};
        
        struct {
            int winner;
            int winner_len;
            double best_score;
        } *all_results = NULL;
        
        if (rank == 0) {
            all_results = (struct {int winner; int winner_len; double best_score;} *)malloc(size * sizeof(*all_results));
        }
        
        MPI_Gather(&local_result, sizeof(local_result), MPI_BYTE,
                   all_results, sizeof(local_result), MPI_BYTE,
                   0, MPI_COMM_WORLD);
        
        if (rank == 0) {
            int global_winner = -1;
            int global_winner_len = 0;
            double global_best = -1e18;
            
            for (int i = 0; i < size; i++) {
                if (all_results[i].winner_len > global_winner_len ||
                    (all_results[i].winner_len == global_winner_len && all_results[i].best_score > global_best)) {
                    global_winner = all_results[i].winner;
                    global_winner_len = all_results[i].winner_len;
                    global_best = all_results[i].best_score;
                }
            }
            
            tmp.total_accepted = global_accepted;
            tmp.total_rejected = global_rejected;
            tmp.fully_accepted = global_fully;
            tmp.winner = global_winner;
            tmp.winner_len = global_winner_len;
            tmp.winner_tokens = &tokens[global_winner * K];
            best = global_best;
            
            free(all_results);
            
            if (repeat == 0)
                res = tmp;
        }
    }
    
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &t1);
        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
        res.elapsed_sec = elapsed / BENCHMARK_REPEAT;
    }
    
    return res;
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    const char *rounds_file = (argc >= 2) ? argv[1] : "rounds.bin";
    const char *vocab_file  = (argc >= 3) ? argv[2] : "gpt2_vocab.txt";
    
    RoundsData *rd = NULL;
    int B, K, R, prompt_len;
    int *prompt = NULL;
    
    if (rank == 0) {
        
        load_vocab(vocab_file);
        if (vocab_len > 0)
            fprintf(stderr, "[info] Vocab: %d entries\n", vocab_len);
        
        rd = load_rounds(rounds_file);
        if (!rd) {
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        
        B = rd->B;
        K = rd->K;
        R = rd->n_rounds;
        prompt_len = rd->prompt_len;
        prompt = rd->prompt;
    }
    
    MPI_Bcast(&B, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&R, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&prompt_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank != 0) {
        prompt = (int *)malloc(prompt_len * sizeof(int));
    }
    
    MPI_Bcast(prompt, prompt_len, MPI_INT, 0, MPI_COMM_WORLD);
    
    int  ctx_cap  = prompt_len + R * K + 64;
    int *ctx_toks = NULL;
    int ctx_len = prompt_len;
    
    if (rank == 0) {
        ctx_toks = (int *)malloc(ctx_cap * sizeof(int));
        if (!ctx_toks) { fprintf(stderr, "OOM\n"); MPI_Abort(MPI_COMM_WORLD, 1); return 1; }
        memcpy(ctx_toks, prompt, prompt_len * sizeof(int));
    }
    
    long   grand_accepted = 0, grand_rejected = 0;
    double grand_elapsed  = 0.0;
    
    if (rank == 0) {
        printf("=== Speculative Decoding Verification — MPI ===\n\n");
        printf("  Draft  model : distilgpt2\n");
        printf("  Target model : gpt2\n");
        printf("  Acceptance   : Chen et al. (2023)  u < min(1, p_target/p_draft)\n");
        printf("  Winner rule  : longest accepted prefix (no global scoring)\n");
        printf("  Commit rule  : rollback-free prefix append\n");
        printf("  Processes    : %d\n", size);
        printf("  Rounds : %d  |  B : %d  |  K : %d\n\n", R, B, K);
        printf("  Prompt: \"");
        print_tokens(prompt, prompt_len);
        printf("\"\n");
        printf("  %s\n\n", "─────────────────────────────────────────────────────");
    }
    
    for (int r = 0; r < R; r++)
    {
        const TokenData *round_tokens = NULL;
        int round_id;
        
        if (rank == 0) {
            round_tokens = rd->rounds[r].tokens;
            round_id = rd->rounds[r].round_id;
        }
        
        int total_tokens = B * K;
        TokenData *local_tokens = NULL;
        
        if (rank != 0) {
            local_tokens = (TokenData *)malloc(total_tokens * sizeof(TokenData));
        } else {
            local_tokens = (TokenData *)round_tokens;
        }
        
        MPI_Bcast(local_tokens, total_tokens * sizeof(TokenData), MPI_BYTE, 0, MPI_COMM_WORLD);
        
        if (rank != 0) {
            round_tokens = local_tokens;
        }
        
        RoundResult res = verify_round_mpi(round_tokens, B, K, rank, size);
        
        if (rank == 0) {
            grand_accepted += res.total_accepted;
            grand_rejected += res.total_rejected;
            grand_elapsed  += res.elapsed_sec;
            
            int committed = res.winner_len;
            for (int k = 0; k < committed && ctx_len < ctx_cap; k++)
                ctx_toks[ctx_len++] = res.winner_tokens[k].token_id;
            
            printf("Round %d/%d\n", r + 1, R);
            printf("  Tokens accepted      : %ld / %ld  (%.1f%%)\n",
                   res.total_accepted, (long)(B * K),
                   100.0 * res.total_accepted / (B * K));
            printf("  Avg accepted / cand  : %.2f / %d\n",
                   (double)res.total_accepted / B, K);
            printf("  Fully accepted       : %d / %d  (%.1f%%)\n",
                   res.fully_accepted, B, 100.0 * res.fully_accepted / B);
            printf("  Avg verification time (%d runs): %.6f s (%.2f µs/candidate)\n", BENCHMARK_REPEAT,
                   res.elapsed_sec, 1e6 * res.elapsed_sec / B);
            
            printf("\n  Winner: candidate #%d  —  %d/%d tokens committed\n",
                   res.winner, committed, K);
            
            for (int k = 0; k < K; k++) {
                const TokenData *td = &round_tokens[res.winner * K + k];
                const char      *w  = detok(td->token_id);
                double ratio = fmin(1.0, exp((double)(td->log_p - td->log_q)));
                
                printf("    [%d] %-6s  [%5d] %-22s  "
                       "log_q=%7.3f  log_p=%7.3f  p/q=%.3f\n",
                       k,
                       k < committed ? "COMMIT" : "STOP",
                       td->token_id,
                       w ? w : "",
                       (double)td->log_q,
                       (double)td->log_p,
                       ratio);
            }
            
            printf("\n  Context (%d tokens): \"", ctx_len);
            print_tokens(ctx_toks, ctx_len);
            printf("\"\n");
            printf("  %s\n\n", "─────────────────────────────────────────────────────");
        }
        
        if (rank != 0) {
            free(local_tokens);
        }
    }
    
    if (rank == 0) {
        long grand_total = grand_accepted + grand_rejected;
        printf("=== Summary (MPI, %d processes) ===\n", size);
        printf("  Total draft tokens     : %ld\n",   grand_total);
        printf("  Tokens committed       : %ld  (%.1f%%)\n",
               grand_accepted, 100.0 * grand_accepted / grand_total);
        printf("  Tokens rejected        : %ld  (%.1f%%)\n",
               grand_rejected, 100.0 * grand_rejected / grand_total);
        printf("  Avg committed / cand   : %.2f / %d\n",
               (double)grand_accepted / (R * B), K);
        printf("  Total verification time: %.6f s\n",   grand_elapsed);
        printf("  Time per candidate     : %.2f µs\n",
               1e6 * grand_elapsed / (R * B));
        printf("\n  Generated text: \"");
        print_tokens(ctx_toks, ctx_len);
        printf("\"\n");
        
        free(ctx_toks);
        free_rounds(rd);
        if (vocab) free(vocab);
    } else {
        free(prompt);
    }
    
    MPI_Finalize();
    return 0;
}
