#include <vector>
#include <omp.h>

double parallel_sum(const std::vector<double>& v) {
    double sum = 0.0;
    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (int i = 0; i < (int)v.size(); i++) {
        sum += v[i];
    }
    return sum;
}
