#include <vector>
#include <omp.h>

double sum_with_false_sharing(const std::vector<double>& v, int nthreads) {
    std::vector<double> partial(nthreads, 0.0);
    int n = (int)v.size();
    #pragma omp parallel num_threads(nthreads)
    {
        int tid = omp_get_thread_num();
        #pragma omp for schedule(static) nowait
        for (int i = 0; i < n; i++) partial[tid] += v[i];
    }
    double sum = 0.0;
    for (double x : partial) sum += x;
    return sum;
}

struct alignas(64) PaddedDouble {
    double val = 0.0;
};

double sum_with_padding(const std::vector<double>& v, int nthreads) {
    std::vector<PaddedDouble> partial(nthreads);
    int n = (int)v.size();
    #pragma omp parallel num_threads(nthreads)
    {
        int tid = omp_get_thread_num();
        #pragma omp for schedule(static) nowait
        for (int i = 0; i < n; i++) partial[tid].val += v[i];
    }
    double sum = 0.0;
    for (auto& p : partial) sum += p.val;
    return sum;
}
