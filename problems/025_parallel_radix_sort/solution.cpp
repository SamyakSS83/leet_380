#include <vector>
#include <numeric>
#include <omp.h>

static void radix_pass(std::vector<unsigned>& in, std::vector<unsigned>& out, int shift) {
    const int BUCKETS = 256;
    int n = (int)in.size();
    int nthreads = omp_get_max_threads();
    std::vector<std::vector<int>> local_hist(nthreads, std::vector<int>(BUCKETS, 0));

    #pragma omp parallel num_threads(nthreads)
    {
        int tid = omp_get_thread_num();
        #pragma omp for schedule(static) nowait
        for (int i = 0; i < n; i++) local_hist[tid][(in[i] >> shift) & 0xFF]++;
    }

    // Merge histograms
    std::vector<int> hist(BUCKETS, 0);
    for (int t = 0; t < nthreads; t++)
        for (int b = 0; b < BUCKETS; b++) hist[b] += local_hist[t][b];

    // Exclusive prefix sum
    std::vector<int> prefix(BUCKETS+1, 0);
    for (int b = 0; b < BUCKETS; b++) prefix[b+1] = prefix[b] + hist[b];

    // Sequential scatter (stable)
    for (int i = 0; i < n; i++) {
        int b = (in[i] >> shift) & 0xFF;
        out[prefix[b]++] = in[i];
    }
}

std::vector<unsigned> parallel_radix_sort(std::vector<unsigned> v) {
    int n = (int)v.size();
    if (n <= 1) return v;
    std::vector<unsigned> tmp(n);
    radix_pass(v, tmp, 0);
    radix_pass(tmp, v, 8);
    radix_pass(v, tmp, 16);
    radix_pass(tmp, v, 24);
    return v;
}
