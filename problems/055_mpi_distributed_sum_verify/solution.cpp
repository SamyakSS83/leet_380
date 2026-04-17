#include <mpi.h>
#include <vector>
#include <cmath>
#include <numeric>

bool verify_distributed_sum(const std::vector<double>& full_array) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Broadcast array size
    int n = (rank == 0) ? (int)full_array.size() : 0;
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int chunk = n / size;

    // Compute reference sum on rank 0
    double ref_sum = 0.0;
    if (rank == 0) {
        for (double v : full_array) ref_sum += v;
    }
    MPI_Bcast(&ref_sum, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Scatter chunks
    std::vector<double> local(chunk);
    MPI_Scatter(
        (rank == 0) ? full_array.data() : nullptr, chunk, MPI_DOUBLE,
        local.data(), chunk, MPI_DOUBLE,
        0, MPI_COMM_WORLD
    );

    // Each rank computes partial sum
    double partial = 0.0;
    for (double v : local) partial += v;

    // Allreduce to get global distributed sum
    double global_sum = 0.0;
    MPI_Allreduce(&partial, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    // Check consistency: global_sum should equal ref_sum within tolerance
    double tol = 1e-9 * (std::fabs(ref_sum) + 1.0);
    return std::fabs(global_sum - ref_sum) <= tol;
}
