#include <vector>
#include <cstdlib>
#include <omp.h>

double monte_carlo_pi(long long n, unsigned seed) {
    int nthreads = omp_get_max_threads();
    std::vector<unsigned> seeds(nthreads);
    for (int i = 0; i < nthreads; i++) seeds[i] = seed + (unsigned)i * 1000003u;

    long long inside = 0;
    #pragma omp parallel reduction(+:inside)
    {
        int tid = omp_get_thread_num();
        unsigned s = seeds[tid];
        #pragma omp for schedule(static)
        for (long long i = 0; i < n; i++) {
            double x = (double)rand_r(&s) / RAND_MAX;
            double y = (double)rand_r(&s) / RAND_MAX;
            if (x*x + y*y < 1.0) inside++;
        }
    }
    return 4.0 * (double)inside / (double)n;
}
