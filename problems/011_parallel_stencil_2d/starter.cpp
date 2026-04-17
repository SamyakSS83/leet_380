#include <vector>
#include <omp.h>

// Apply 2D Jacobi stencil for iters iterations on NxN grid (flat row-major).
// new[i][j] = (old[i-1][j]+old[i+1][j]+old[i][j-1]+old[i][j+1]+old[i][j])/5.0
// Boundary points are fixed.
std::vector<double> stencil_2d(std::vector<double> u, int N, int iters) {
    // TODO: implement
    return u;
}
