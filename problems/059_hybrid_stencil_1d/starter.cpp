#include <mpi.h>
#include <omp.h>
#include <vector>

// 1D Jacobi stencil with MPI ghost cell exchange and OMP sweep.
// local_u: interior + ghost cells (local_u[0] and local_u.back() are ghosts).
// iterations: number of sweeps.
// Returns full interior on rank 0, empty on others.
std::vector<double> hybrid_stencil_1d(
    std::vector<double> local_u, int iterations) {
    // TODO: implement
    return {};
}
