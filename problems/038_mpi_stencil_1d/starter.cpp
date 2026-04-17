#include <mpi.h>
#include <vector>

// Apply 1D Jacobi stencil for iters iterations on distributed data.
// Exchange ghost cells with neighbors. Return updated local chunk (no ghosts).
std::vector<double> mpi_stencil_1d(std::vector<double> local_data, int iters) {
    // TODO: implement with ghost cell exchange
    return local_data;
}
