#include <omp.h>

static long long seq_fib(int n) {
    if (n <= 1) return n;
    long long a = 0, b = 1;
    for (int i = 2; i <= n; i++) { long long c = a+b; a=b; b=c; }
    return b;
}

static long long task_fib(int n, int cutoff) {
    if (n <= cutoff) return seq_fib(n);
    long long x = 0;
    // Spawn one task for n-1; compute n-2 in current task to avoid stack aliasing
    #pragma omp task shared(x) firstprivate(n, cutoff)
    x = task_fib(n-1, cutoff);
    long long y = task_fib(n-2, cutoff);
    #pragma omp taskwait
    return x + y;
}

long long parallel_fibonacci(int n, int cutoff) {
    long long result = 0;
    #pragma omp parallel
    {
        #pragma omp single
        result = task_fib(n, cutoff);
    }
    return result;
}
