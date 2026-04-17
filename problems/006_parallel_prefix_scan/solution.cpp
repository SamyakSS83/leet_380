#include <vector>
#include <omp.h>

std::vector<double> parallel_prefix_scan(const std::vector<double>& v) {
    int n = (int)v.size();
    if (n == 0) return {};
    std::vector<double> out(n);
    int nthreads = 1;

    #pragma omp parallel
    {
        #pragma omp single
        nthreads = omp_get_num_threads();
    }

    std::vector<double> chunk_sum(nthreads, 0.0);

    #pragma omp parallel num_threads(nthreads)
    {
        int tid = omp_get_thread_num();
        int chunk = (n + nthreads - 1) / nthreads;
        int start = tid * chunk;
        int end = std::min(start + chunk, n);

        // Phase 1: local inclusive scan
        if (start < end) {
            out[start] = v[start];
            for (int i = start + 1; i < end; i++)
                out[i] = out[i-1] + v[i];
            chunk_sum[tid] = out[end - 1];
        }
        #pragma omp barrier

        // Phase 2: single thread computes prefix offsets
        #pragma omp single
        {
            double acc = 0.0;
            for (int t = 0; t < nthreads; t++) {
                double s = chunk_sum[t];
                chunk_sum[t] = acc;
                acc += s;
            }
        }

        // Phase 3: add offset to chunk
        double offset = chunk_sum[tid];
        if (offset != 0.0) {
            for (int i = start; i < end; i++)
                out[i] += offset;
        }
    }
    return out;
}
