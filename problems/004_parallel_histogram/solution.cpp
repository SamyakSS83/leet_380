#include <vector>
#include <omp.h>

std::vector<int> parallel_histogram(const std::vector<int>& v, int B) {
    std::vector<int> hist(B, 0);
    int n = (int)v.size();
    #pragma omp parallel
    {
        std::vector<int> local_hist(B, 0);
        #pragma omp for schedule(static) nowait
        for (int i = 0; i < n; i++) {
            local_hist[v[i] % B]++;
        }
        #pragma omp critical
        for (int b = 0; b < B; b++) hist[b] += local_hist[b];
    }
    return hist;
}
