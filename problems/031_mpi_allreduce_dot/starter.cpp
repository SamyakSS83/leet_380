#include <mpi.h>
#include <vector>

// Compute global dot product of distributed vectors using MPI_Allreduce.
// All ranks return the same global dot product.
double mpi_allreduce_dot(const std::vector<double>& local_a,
                          const std::vector<double>& local_b) {
    // TODO: implement
    return 0.0;
}
