#include <mpi.h>
#include <vector>
#include <numeric>

double mpi_distributed_sum(const std::vector<double>& local_data) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double local_sum = std::accumulate(local_data.begin(), local_data.end(), 0.0);
    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    return (rank == 0) ? global_sum : 0.0;
}
