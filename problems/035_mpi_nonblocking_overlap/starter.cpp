#include <mpi.h>
#include <vector>
#include <utility>

// Non-blocking send local_data to rank 0 while computing local sum-of-squares.
// Rank 0 collects all data. Returns {local_sos, all_data (rank 0) or empty}.
std::pair<double, std::vector<std::vector<double>>>
    mpi_nonblocking_collect(const std::vector<double>& local_data) {
    // TODO: implement using MPI_Isend/MPI_Irecv
    return {0.0, {}};
}
