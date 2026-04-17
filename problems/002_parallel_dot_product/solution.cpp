#include <vector>
#include <omp.h>

double parallel_dot_product(const std::vector<double>& a, const std::vector<double>& b) {
    double dot = 0.0;
    int n = (int)a.size();
    #pragma omp parallel for reduction(+:dot) schedule(static)
    for (int i = 0; i < n; i++) {
        dot += a[i] * b[i];
    }
    return dot;
}
