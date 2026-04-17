#include <mpi.h>
#include <vector>

// Compute global histogram with B bins using MPI_Allreduce.
// bin[i] = count of elements v where v%B == i, across all ranks.
std::vector<int> mpi_histogram(const std::vector<int>& local_data, int B) {
    // TODO: implement
    return std::vector<int>(B, 0);
}
