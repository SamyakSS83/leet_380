#include <vector>
#include <omp.h>

void parallel_saxpy(double a, const std::vector<double>& x, std::vector<double>& y) {
    int n = (int)x.size();
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++) y[i] = a * x[i] + y[i];
}
