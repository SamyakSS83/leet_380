#include <mpi.h>
#include <vector>

// Row-distributed matvec: each rank has local_nrows rows of A (flat row-major).
// x_full is provided on rank 0; broadcast and compute y = A*x per rank.
// Gather results to rank 0 and return full y (empty on other ranks).
std::vector<double> mpi_matvec(
    const std::vector<double>& local_rows_flat,
    const std::vector<double>& x_full,
    int local_nrows, int ncols) {
    // TODO: implement
    return {};
}
