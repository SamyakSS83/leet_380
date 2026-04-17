#include <vector>
#include <omp.h>

std::vector<double> stencil_1d(std::vector<double> u, int iters) {
    int n = (int)u.size();
    if (n < 3 || iters == 0) return u;
    std::vector<double> tmp(n);
    tmp[0] = u[0]; tmp[n-1] = u[n-1];
    for (int iter = 0; iter < iters; iter++) {
        #pragma omp parallel for schedule(static)
        for (int i = 1; i < n-1; i++) {
            tmp[i] = (u[i-1] + u[i] + u[i+1]) / 3.0;
        }
        std::swap(u, tmp);
        tmp[0] = u[0]; tmp[n-1] = u[n-1];
    }
    return u;
}
