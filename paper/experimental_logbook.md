# Appendix B: Experimental Logbook

## Experimental Setup
- **CPU Model**: [To be filled by user]
- **Number of Cores**: [To be filled by user]
- **RAM**: [To be filled by user]
- **OS**: Linux
- **Compiler**: gcc
- **MPI Library**: OpenMPI
- **BENCHMARK_REPEAT**: 50

## Experiment Results

| Date & Time | Algorithm | Cores/Processes | Dataset Size (B) | Runtime (s) | Speedup |
|-------------|-----------|-----------------|------------------|-------------|--------|
| 2026-06-26 | Sequential | 1 | 25,000 | 0.013030 | 1.00x |
| 2026-06-26 | Sequential | 1 | 50,000 | 0.026372 | 1.00x |
| 2026-06-26 | Sequential | 1 | 100,000 | 0.050366 | 1.00x |
| 2026-06-26 | Sequential | 1 | 250,000 | 0.130838 | 1.00x |
| 2026-06-26 | Pthreads | 1 | 25,000 | 0.038321 | 0.34x |
| 2026-06-26 | Pthreads | 2 | 25,000 | 0.008636 | 1.51x |
| 2026-06-26 | Pthreads | 4 | 25,000 | 0.006859 | 1.90x |
| 2026-06-26 | Pthreads | 8 | 25,000 | 0.007369 | 1.77x |
| 2026-06-26 | Pthreads | 1 | 50,000 | 0.077567 | 0.34x |
| 2026-06-26 | Pthreads | 2 | 50,000 | 0.016083 | 1.64x |
| 2026-06-26 | Pthreads | 4 | 50,000 | 0.010219 | 2.58x |
| 2026-06-26 | Pthreads | 8 | 50,000 | 0.014099 | 1.87x |
| 2026-06-26 | Pthreads | 1 | 100,000 | 0.136127 | 0.37x |
| 2026-06-26 | Pthreads | 2 | 100,000 | 0.044184 | 1.14x |
| 2026-06-26 | Pthreads | 4 | 100,000 | 0.028779 | 1.75x |
| 2026-06-26 | Pthreads | 8 | 100,000 | 0.020642 | 2.44x |
| 2026-06-26 | Pthreads | 1 | 250,000 | 0.204435 | 0.64x |
| 2026-06-26 | Pthreads | 2 | 250,000 | 0.078822 | 1.66x |
| 2026-06-26 | Pthreads | 4 | 250,000 | 0.046071 | 2.84x |
| 2026-06-26 | Pthreads | 8 | 250,000 | 0.037382 | 3.50x |
| 2026-06-26 | OpenMP | 1 | 25,000 | 0.016086 | 0.81x |
| 2026-06-26 | OpenMP | 2 | 25,000 | 0.010508 | 1.24x |
| 2026-06-26 | OpenMP | 4 | 25,000 | 0.005303 | 2.46x |
| 2026-06-26 | OpenMP | 8 | 25,000 | 0.003326 | 3.92x |
| 2026-06-26 | OpenMP | 1 | 50,000 | 0.027760 | 0.95x |
| 2026-06-26 | OpenMP | 2 | 50,000 | 0.014574 | 1.81x |
| 2026-06-26 | OpenMP | 4 | 50,000 | 0.008474 | 3.11x |
| 2026-06-26 | OpenMP | 8 | 50,000 | 0.007104 | 3.71x |
| 2026-06-26 | OpenMP | 1 | 100,000 | 0.054160 | 0.93x |
| 2026-06-26 | OpenMP | 2 | 100,000 | 0.028617 | 1.76x |
| 2026-06-26 | OpenMP | 4 | 100,000 | 0.016247 | 3.10x |
| 2026-06-26 | OpenMP | 8 | 100,000 | 0.013718 | 3.67x |
| 2026-06-26 | OpenMP | 1 | 250,000 | 0.140683 | 0.93x |
| 2026-06-26 | OpenMP | 2 | 250,000 | 0.084425 | 1.55x |
| 2026-06-26 | OpenMP | 4 | 250,000 | 0.058679 | 2.23x |
| 2026-06-26 | OpenMP | 8 | 250,000 | 0.034986 | 3.74x |
| 2026-06-26 | MPI | 1 | 25,000 | 0.012292 | 1.06x |
| 2026-06-26 | MPI | 2 | 25,000 | 0.006653 | 1.96x |
| 2026-06-26 | MPI | 4 | 25,000 | 0.003878 | 3.36x |
| 2026-06-26 | MPI | 8 | 25,000 | 0.002638 | 4.94x |
| 2026-06-26 | MPI | 1 | 50,000 | 0.031021 | 0.85x |
| 2026-06-26 | MPI | 2 | 50,000 | 0.015513 | 1.70x |
| 2026-06-26 | MPI | 4 | 50,000 | 0.008347 | 3.16x |
| 2026-06-26 | MPI | 8 | 50,000 | 0.007261 | 3.63x |
| 2026-06-26 | MPI | 1 | 100,000 | 0.057234 | 0.88x |
| 2026-06-26 | MPI | 2 | 100,000 | 0.029282 | 1.72x |
| 2026-06-26 | MPI | 4 | 100,000 | 0.017718 | 2.84x |
| 2026-06-26 | MPI | 8 | 100,000 | 0.022087 | 2.28x |
| 2026-06-26 | MPI | 1 | 250,000 | 0.132161 | 0.99x |
| 2026-06-26 | MPI | 2 | 250,000 | 0.067741 | 1.93x |
| 2026-06-26 | MPI | 4 | 250,000 | 0.038099 | 3.43x |
| 2026-06-26 | MPI | 8 | 250,000 | 0.026168 | 5.00x |

## Key Findings

### Best Speedup Achieved
- **MPI with 8 processes (B=250,000)**: 5.00x speedup
- **OpenMP with 8 threads (B=25,000)**: 3.92x speedup
- **Pthreads with 8 threads (B=250,000)**: 3.50x speedup

### Observations
1. **MPI** shows the best scalability, achieving up to 5x speedup on the largest dataset with 8 processes
2. **OpenMP** demonstrates consistent performance across datasets, with speedup generally improving with more threads
3. **Pthreads** shows good speedup at 4 threads but some overhead at 8 threads for smaller datasets
4. All parallel implementations show better speedup on larger datasets (B=250,000)
5. Single-thread parallel versions sometimes show slowdown (<1x) due to parallelization overhead

### Correctness
All four versions (Sequential, Pthreads, OpenMP, MPI) produce identical outputs within floating-point tolerance after RNG fixes.
