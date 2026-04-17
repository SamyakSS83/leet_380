#include <mpi.h>
#include <vector>

// Scatter data from rank 0, square elements, gather back.
// data is n doubles on rank 0 (n divisible by nprocs).
// Returns squared array on rank 0.
std::vector<double> scatter_square_gather(std::vector<double> data) {
    // TODO: implement using MPI_Scatter and MPI_Gather
    return {};
}
