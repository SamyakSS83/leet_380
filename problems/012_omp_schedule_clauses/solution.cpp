#include <vector>
#include <cmath>
#include <omp.h>

double irregular_sum(const std::vector<int>& work_sizes, double base) {
    double sum = 0.0;
    int n = (int)work_sizes.size();
    #pragma omp parallel for schedule(dynamic, 8) reduction(+:sum)
    for (int i = 0; i < n; i++) {
        sum += std::pow(base, work_sizes[i]);
    }
    return sum;
}
