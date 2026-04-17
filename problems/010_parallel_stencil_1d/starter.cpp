#include <vector>
#include <omp.h>

// Apply 1D Jacobi stencil for iters iterations.
// new[i] = (u[i-1] + u[i] + u[i+1]) / 3.0, boundaries are fixed.
std::vector<double> stencil_1d(std::vector<double> u, int iters) {
    // TODO: implement
    return u;
}
