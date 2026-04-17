#include <vector>
#include <omp.h>

std::vector<double> stencil_2d(std::vector<double> u, int N, int iters) {
    if (N < 3 || iters == 0) return u;
    std::vector<double> tmp(u);
    for (int iter = 0; iter < iters; iter++) {
        #pragma omp parallel for schedule(static)
        for (int i = 1; i < N-1; i++) {
            for (int j = 1; j < N-1; j++) {
                tmp[i*N+j] = (u[(i-1)*N+j] + u[(i+1)*N+j] +
                               u[i*N+(j-1)] + u[i*N+(j+1)] + u[i*N+j]) / 5.0;
            }
        }
        std::swap(u, tmp);
    }
    return u;
}
