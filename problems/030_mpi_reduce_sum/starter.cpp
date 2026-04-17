#include <mpi.h>
#include <vector>

// Each rank sums its local_data, then MPI_Reduce to get global sum at rank 0.
double mpi_distributed_sum(const std::vector<double>& local_data) {
    // TODO: implement
    return 0.0;
}
