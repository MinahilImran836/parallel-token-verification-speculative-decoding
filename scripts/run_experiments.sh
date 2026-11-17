#!/bin/bash

# Parallel Token Verification Engine - Experiment Script
# This script compiles and runs all four versions with different configurations

set -e  # Exit on error

# Configuration
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC_DIR="$PROJECT_ROOT/src"
RESULTS_DIR="$PROJECT_ROOT/results/raw_outputs"
DATA_DIR="$PROJECT_ROOT/data"

# Test data files (using all 4 dataset sizes)
ROUNDS_25K="$DATA_DIR/rounds_25000.bin"
ROUNDS_50K="$DATA_DIR/rounds_50000.bin"
ROUNDS_100K="$DATA_DIR/rounds_100000.bin"
ROUNDS_250K="$DATA_DIR/rounds_250000.bin"
VOCAB_FILE="$DATA_DIR/gpt2_vocab.txt"

# Create results directory
mkdir -p "$RESULTS_DIR"

echo "========================================="
echo "Parallel Token Verification Experiments"
echo "========================================="
echo "Project root: $PROJECT_ROOT"
echo "Results directory: $RESULTS_DIR"
echo ""

# Function to compile sequential version
compile_sequential() {
    echo "[1/4] Compiling Sequential version..."
    cd "$SRC_DIR/sequential"
    gcc -O3 main_seq.c -o main_seq -lm
    echo "✓ Sequential compiled successfully"
    echo ""
}

# Function to compile pthreads version
compile_pthreads() {
    echo "[2/4] Compiling Pthreads version..."
    cd "$SRC_DIR/pthreads"
    gcc -O3 main_pth.c -o main_pth -lm -pthread
    echo "✓ Pthreads compiled successfully"
    echo ""
}

# Function to compile OpenMP version
compile_openmp() {
    echo "[3/4] Compiling OpenMP version..."
    cd "$SRC_DIR/openmp"
    gcc -O3 main_omp.c -o main_omp -lm -fopenmp
    echo "✓ OpenMP compiled successfully"
    echo ""
}

# Function to compile MPI version
compile_mpi() {
    echo "[4/4] Compiling MPI version..."
    cd "$SRC_DIR/mpi"
    mpicc -O3 main_mpi.c -o main_mpi -lm
    echo "✓ MPI compiled successfully"
    echo ""
}

# Function to run sequential experiments
run_sequential() {
    echo "========================================="
    echo "Running Sequential Experiments"
    echo "========================================="
    
    cd "$SRC_DIR/sequential"
    
    # Run with all 4 dataset sizes
    echo "Test 1: Sequential with rounds_25000.bin"
    ./main_seq "$ROUNDS_25K" "$VOCAB_FILE" > "$RESULTS_DIR/seq_25k.txt" 2>&1
    echo "✓ Saved to $RESULTS_DIR/seq_25k.txt"
    
    echo "Test 2: Sequential with rounds_50000.bin"
    ./main_seq "$ROUNDS_50K" "$VOCAB_FILE" > "$RESULTS_DIR/seq_50k.txt" 2>&1
    echo "✓ Saved to $RESULTS_DIR/seq_50k.txt"
    
    echo "Test 3: Sequential with rounds_100000.bin"
    ./main_seq "$ROUNDS_100K" "$VOCAB_FILE" > "$RESULTS_DIR/seq_100k.txt" 2>&1
    echo "✓ Saved to $RESULTS_DIR/seq_100k.txt"
    
    echo "Test 4: Sequential with rounds_250000.bin"
    ./main_seq "$ROUNDS_250K" "$VOCAB_FILE" > "$RESULTS_DIR/seq_250k.txt" 2>&1
    echo "✓ Saved to $RESULTS_DIR/seq_250k.txt"
    echo ""
}

# Function to run pthreads experiments
run_pthreads() {
    echo "========================================="
    echo "Running Pthreads Experiments"
    echo "========================================="
    
    cd "$SRC_DIR/pthreads"
    
    # Test with different thread counts and all dataset sizes
    for threads in 1 2 4 8; do
        echo "Test: Pthreads with $threads threads (rounds_25000.bin)"
        ./main_pth "$ROUNDS_25K" "$VOCAB_FILE" $threads > "$RESULTS_DIR/pth_${threads}t_25k.txt" 2>&1
        echo "✓ Saved to $RESULTS_DIR/pth_${threads}t_25k.txt"
    done
    
    for threads in 1 2 4 8; do
        echo "Test: Pthreads with $threads threads (rounds_50000.bin)"
        ./main_pth "$ROUNDS_50K" "$VOCAB_FILE" $threads > "$RESULTS_DIR/pth_${threads}t_50k.txt" 2>&1
        echo "✓ Saved to $RESULTS_DIR/pth_${threads}t_50k.txt"
    done
    
    for threads in 1 2 4 8; do
        echo "Test: Pthreads with $threads threads (rounds_100000.bin)"
        ./main_pth "$ROUNDS_100K" "$VOCAB_FILE" $threads > "$RESULTS_DIR/pth_${threads}t_100k.txt" 2>&1
        echo "✓ Saved to $RESULTS_DIR/pth_${threads}t_100k.txt"
    done
    
    for threads in 1 2 4 8; do
        echo "Test: Pthreads with $threads threads (rounds_250000.bin)"
        ./main_pth "$ROUNDS_250K" "$VOCAB_FILE" $threads > "$RESULTS_DIR/pth_${threads}t_250k.txt" 2>&1
        echo "✓ Saved to $RESULTS_DIR/pth_${threads}t_250k.txt"
    done
    echo ""
}

# Function to run OpenMP experiments
run_openmp() {
    echo "========================================="
    echo "Running OpenMP Experiments"
    echo "========================================="
    
    cd "$SRC_DIR/openmp"
    
    # Test with different thread counts and all dataset sizes
    for threads in 1 2 4 8; do
        echo "Test: OpenMP with $threads threads (rounds_25000.bin)"
        ./main_omp "$ROUNDS_25K" "$VOCAB_FILE" $threads > "$RESULTS_DIR/omp_${threads}t_25k.txt" 2>&1
        echo "✓ Saved to $RESULTS_DIR/omp_${threads}t_25k.txt"
    done
    
    for threads in 1 2 4 8; do
        echo "Test: OpenMP with $threads threads (rounds_50000.bin)"
        ./main_omp "$ROUNDS_50K" "$VOCAB_FILE" $threads > "$RESULTS_DIR/omp_${threads}t_50k.txt" 2>&1
        echo "✓ Saved to $RESULTS_DIR/omp_${threads}t_50k.txt"
    done
    
    for threads in 1 2 4 8; do
        echo "Test: OpenMP with $threads threads (rounds_100000.bin)"
        ./main_omp "$ROUNDS_100K" "$VOCAB_FILE" $threads > "$RESULTS_DIR/omp_${threads}t_100k.txt" 2>&1
        echo "✓ Saved to $RESULTS_DIR/omp_${threads}t_100k.txt"
    done
    
    for threads in 1 2 4 8; do
        echo "Test: OpenMP with $threads threads (rounds_250000.bin)"
        ./main_omp "$ROUNDS_250K" "$VOCAB_FILE" $threads > "$RESULTS_DIR/omp_${threads}t_250k.txt" 2>&1
        echo "✓ Saved to $RESULTS_DIR/omp_${threads}t_250k.txt"
    done
    echo ""
}

# Function to run MPI experiments
run_mpi() {
    echo "========================================="
    echo "Running MPI Experiments"
    echo "========================================="
    
    cd "$SRC_DIR/mpi"
    
    # Test with different process counts and all dataset sizes
    for procs in 1 2 4 8; do
        echo "Test: MPI with $procs processes (rounds_25000.bin)"
        if [ $procs -le 4 ]; then
            mpirun -np $procs ./main_mpi "$ROUNDS_25K" "$VOCAB_FILE" > "$RESULTS_DIR/mpi_${procs}p_25k.txt" 2>&1
        else
            mpirun --oversubscribe -np $procs ./main_mpi "$ROUNDS_25K" "$VOCAB_FILE" > "$RESULTS_DIR/mpi_${procs}p_25k.txt" 2>&1
        fi
        echo "✓ Saved to $RESULTS_DIR/mpi_${procs}p_25k.txt"
    done
    
    for procs in 1 2 4 8; do
        echo "Test: MPI with $procs processes (rounds_50000.bin)"
        if [ $procs -le 4 ]; then
            mpirun -np $procs ./main_mpi "$ROUNDS_50K" "$VOCAB_FILE" > "$RESULTS_DIR/mpi_${procs}p_50k.txt" 2>&1
        else
            mpirun --oversubscribe -np $procs ./main_mpi "$ROUNDS_50K" "$VOCAB_FILE" > "$RESULTS_DIR/mpi_${procs}p_50k.txt" 2>&1
        fi
        echo "✓ Saved to $RESULTS_DIR/mpi_${procs}p_50k.txt"
    done
    
    for procs in 1 2 4 8; do
        echo "Test: MPI with $procs processes (rounds_100000.bin)"
        if [ $procs -le 4 ]; then
            mpirun -np $procs ./main_mpi "$ROUNDS_100K" "$VOCAB_FILE" > "$RESULTS_DIR/mpi_${procs}p_100k.txt" 2>&1
        else
            mpirun --oversubscribe -np $procs ./main_mpi "$ROUNDS_100K" "$VOCAB_FILE" > "$RESULTS_DIR/mpi_${procs}p_100k.txt" 2>&1
        fi
        echo "✓ Saved to $RESULTS_DIR/mpi_${procs}p_100k.txt"
    done
    
    for procs in 1 2 4 8; do
        echo "Test: MPI with $procs processes (rounds_250000.bin)"
        if [ $procs -le 4 ]; then
            mpirun -np $procs ./main_mpi "$ROUNDS_250K" "$VOCAB_FILE" > "$RESULTS_DIR/mpi_${procs}p_250k.txt" 2>&1
        else
            mpirun --oversubscribe -np $procs ./main_mpi "$ROUNDS_250K" "$VOCAB_FILE" > "$RESULTS_DIR/mpi_${procs}p_250k.txt" 2>&1
        fi
        echo "✓ Saved to $RESULTS_DIR/mpi_${procs}p_250k.txt"
    done
    echo ""
}

# Main execution
main() {
    echo "Starting compilation..."
    echo ""
    
    compile_sequential
    compile_pthreads
    compile_openmp
    compile_mpi
    
    echo "========================================="
    echo "Compilation Complete"
    echo "========================================="
    echo ""
    
    echo "Starting experiments..."
    echo ""
    
    run_sequential
    run_pthreads
    run_openmp
    run_mpi
    
    echo "========================================="
    echo "All Experiments Complete"
    echo "========================================="
    echo "Results saved to: $RESULTS_DIR"
    echo ""
    echo "Generated files:"
    ls -lh "$RESULTS_DIR"
}

# Run main function
main
