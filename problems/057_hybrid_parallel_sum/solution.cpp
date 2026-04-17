#include <mpi.h>
#include <omp.h>
#include <vector>

double hybrid_sum(const std::vector<double>& data) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Broadcast total size
    int n = (rank == 0) ? (int)data.size() : 0;
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int chunk = n / size;
    std::vector<double> local(chunk);

    // Scatter chunks from rank 0
    MPI_Scatter(
        (rank == 0) ? data.data() : nullptr, chunk, MPI_DOUBLE,
        local.data(), chunk, MPI_DOUBLE,
        0, MPI_COMM_WORLD
    );

    // OMP parallel reduction over local chunk
    double local_sum = 0.0;
    #pragma omp parallel for reduction(+:local_sum)
    for (int i = 0; i < chunk; i++) {
        local_sum += local[i];
    }

    // MPI reduction to rank 0
    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    return (rank == 0) ? global_sum : 0.0;
}
