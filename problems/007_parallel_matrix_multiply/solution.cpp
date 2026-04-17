#include <vector>
#include <omp.h>

std::vector<double> parallel_matmul(const std::vector<double>& A,
                                     const std::vector<double>& B, int N) {
    std::vector<double> C(N * N, 0.0);
    if (N == 0) return C;
    #pragma omp parallel for schedule(static) collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += A[i*N + k] * B[k*N + j];
            }
            C[i*N + j] = sum;
        }
    }
    return C;
}
