#include <vector>
#include <omp.h>

std::vector<double> parallel_matvec(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& x) {
    int m = (int)A.size();
    if (m == 0) return {};
    int n = (int)x.size();
    std::vector<double> y(m, 0.0);
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < m; i++) {
        double sum = 0.0;
        for (int j = 0; j < n; j++) {
            sum += A[i][j] * x[j];
        }
        y[i] = sum;
    }
    return y;
}
