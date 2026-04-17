#include <mpi.h>
#include <omp.h>
#include <vector>

std::vector<double> hybrid_matmul(
    const std::vector<double>& local_A, int local_rows,
    const std::vector<double>& B, int n, int k, int m) {

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Compute local rows of C = local_A * B using OMP
    std::vector<double> local_C(local_rows * m, 0.0);

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < local_rows; i++) {
        for (int j = 0; j < m; j++) {
            double sum = 0.0;
            for (int p = 0; p < k; p++) {
                sum += local_A[i * k + p] * B[p * m + j];
            }
            local_C[i * m + j] = sum;
        }
    }

    // Gather row counts from all ranks
    std::vector<int> recv_counts(size), displs(size);
    int send_count = local_rows * m;
    MPI_Gather(&send_count, 1, MPI_INT, recv_counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<double> full_C;
    if (rank == 0) {
        displs[0] = 0;
        for (int i = 1; i < size; i++) displs[i] = displs[i-1] + recv_counts[i-1];
        full_C.resize(n * m);
    }

    MPI_Gatherv(local_C.data(), send_count, MPI_DOUBLE,
                full_C.data(), recv_counts.data(), displs.data(), MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    return full_C;
}
