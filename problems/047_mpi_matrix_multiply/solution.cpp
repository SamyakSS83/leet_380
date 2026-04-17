#include <mpi.h>
#include <vector>

std::vector<double> distributed_matmul(
    const std::vector<double>& local_A, int local_rows,
    const std::vector<double>& B, int n, int k, int m) {

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Compute local rows of C
    std::vector<double> local_C(local_rows * m, 0.0);
    for (int i = 0; i < local_rows; i++) {
        for (int p = 0; p < k; p++) {
            double a = local_A[i * k + p];
            for (int j = 0; j < m; j++) {
                local_C[i * m + j] += a * B[p * m + j];
            }
        }
    }

    // Gather to rank 0
    if (rank == 0) {
        std::vector<double> C(n * m);
        MPI_Gather(local_C.data(), local_rows * m, MPI_DOUBLE,
                   C.data(), local_rows * m, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        return C;
    } else {
        MPI_Gather(local_C.data(), local_rows * m, MPI_DOUBLE,
                   nullptr, local_rows * m, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        return {};
    }
}
