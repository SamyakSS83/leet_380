#include <omp.h>

// Compute Fibonacci(n) using OMP tasks with cutoff for small n.
// Use sequential computation for n < cutoff.
long long parallel_fibonacci(int n, int cutoff);

// Helper: sequential fib
static long long seq_fib(int n) {
    if (n <= 1) return n;
    return seq_fib(n-1) + seq_fib(n-2);
}

long long parallel_fibonacci(int n, int cutoff) {
    // TODO: implement using omp tasks
    return 0;
}
