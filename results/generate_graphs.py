import matplotlib.pyplot as plt
import numpy as np

# Data from the user's speedup results
datasets = ['25,000', '50,000', '100,000', '250,000']
threads = [1, 2, 4, 8]

# Pthreads Speedup
pthread_speedup = {
    1: [0.34, 0.34, 0.37, 0.64],
    2: [1.51, 1.64, 1.14, 1.66],
    4: [1.90, 2.58, 1.75, 2.84],
    8: [1.77, 1.87, 2.44, 3.50]
}

# OpenMP Speedup
openmp_speedup = {
    1: [0.81, 0.95, 0.93, 0.93],
    2: [1.24, 1.81, 1.76, 1.55],
    4: [2.46, 3.11, 3.10, 2.23],
    8: [3.92, 3.71, 3.67, 3.74]
}

# MPI Speedup
mpi_speedup = {
    1: [1.06, 0.85, 0.88, 0.99],
    2: [1.96, 1.70, 1.72, 1.93],
    4: [3.36, 3.16, 2.84, 3.43],
    8: [4.94, 3.63, 2.28, 5.00]
}

# Create figure with subplots
fig, axes = plt.subplots(1, 3, figsize=(18, 5))

# Pthreads Graph
ax1 = axes[0]
x = np.arange(len(datasets))
width = 0.2
for i, t in enumerate(threads):
    ax1.bar(x + i*width, pthread_speedup[t], width, label=f'{t} thread(s)')
ax1.set_xlabel('Dataset Size (B)')
ax1.set_ylabel('Speedup')
ax1.set_title('Pthreads Speedup')
ax1.set_xticks(x + width * 1.5)
ax1.set_xticklabels(datasets)
ax1.legend()
ax1.grid(axis='y', alpha=0.3)
ax1.axhline(y=1, color='r', linestyle='--', alpha=0.5, label='Baseline')

# OpenMP Graph
ax2 = axes[1]
for i, t in enumerate(threads):
    ax2.bar(x + i*width, openmp_speedup[t], width, label=f'{t} thread(s)')
ax2.set_xlabel('Dataset Size (B)')
ax2.set_ylabel('Speedup')
ax2.set_title('OpenMP Speedup')
ax2.set_xticks(x + width * 1.5)
ax2.set_xticklabels(datasets)
ax2.legend()
ax2.grid(axis='y', alpha=0.3)
ax2.axhline(y=1, color='r', linestyle='--', alpha=0.5)

# MPI Graph
ax3 = axes[2]
for i, t in enumerate(threads):
    ax3.bar(x + i*width, mpi_speedup[t], width, label=f'{t} process(es)')
ax3.set_xlabel('Dataset Size (B)')
ax3.set_ylabel('Speedup')
ax3.set_title('MPI Speedup')
ax3.set_xticks(x + width * 1.5)
ax3.set_xticklabels(datasets)
ax3.legend()
ax3.grid(axis='y', alpha=0.3)
ax3.axhline(y=1, color='r', linestyle='--', alpha=0.5)

plt.tight_layout()
plt.savefig('speedup_comparison.png', dpi=300, bbox_inches='tight')
print("Speedup comparison graph saved to speedup_comparison.png")

# Strong Scaling Graph (Speedup vs Threads for largest dataset)
fig2, ax = plt.subplots(figsize=(10, 6))
pthread_vals = [pthread_speedup[t][3] for t in threads]
openmp_vals = [openmp_speedup[t][3] for t in threads]
mpi_vals = [mpi_speedup[t][3] for t in threads]
ax.plot(threads, pthread_vals, 'o-', label='Pthreads', linewidth=2, markersize=8)
ax.plot(threads, openmp_vals, 's-', label='OpenMP', linewidth=2, markersize=8)
ax.plot(threads, mpi_vals, '^-', label='MPI', linewidth=2, markersize=8)
ax.plot(threads, threads, 'k--', label='Ideal Speedup', alpha=0.5)
ax.set_xlabel('Number of Threads/Processes')
ax.set_ylabel('Speedup')
ax.set_title('Strong Scaling: Speedup vs Threads (B=250,000)')
ax.set_xticks(threads)
ax.legend()
ax.grid(True, alpha=0.3)
plt.savefig('strong_scaling.png', dpi=300, bbox_inches='tight')
print("Strong scaling graph saved to strong_scaling.png")

# Combined Strong Scaling for all datasets
fig3, ax = plt.subplots(figsize=(12, 6))
for i, dataset in enumerate(datasets):
    pthread_vals = [pthread_speedup[t][i] for t in threads]
    openmp_vals = [openmp_speedup[t][i] for t in threads]
    mpi_vals = [mpi_speedup[t][i] for t in threads]
    
    if i == 0:
        ax.plot(threads, pthread_vals, 'o-', label=f'Pthreads (B={dataset})', linewidth=2, markersize=6)
        ax.plot(threads, openmp_vals, 's-', label=f'OpenMP (B={dataset})', linewidth=2, markersize=6)
        ax.plot(threads, mpi_vals, '^-', label=f'MPI (B={dataset})', linewidth=2, markersize=6)
    else:
        ax.plot(threads, pthread_vals, 'o-', linewidth=2, markersize=6)
        ax.plot(threads, openmp_vals, 's-', linewidth=2, markersize=6)
        ax.plot(threads, mpi_vals, '^-', linewidth=2, markersize=6)

ax.plot(threads, threads, 'k--', label='Ideal Speedup', alpha=0.5, linewidth=2)
ax.set_xlabel('Number of Threads/Processes')
ax.set_ylabel('Speedup')
ax.set_title('Strong Scaling: Speedup vs Threads (All Datasets)')
ax.set_xticks(threads)
ax.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('strong_scaling_all.png', dpi=300, bbox_inches='tight')
print("Strong scaling graph (all datasets) saved to strong_scaling_all.png")

print("\nAll graphs generated successfully!")
