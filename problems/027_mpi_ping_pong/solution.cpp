#include <mpi.h>
#include <vector>

double mpi_ping_pong(int N, int msg_size) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::vector<double> buf(msg_size, (double)rank);
    MPI_Status status;
    double t0 = 0, t1 = 0;

    if (rank == 0) {
        t0 = MPI_Wtime();
        for (int i = 0; i < N; i++) {
            MPI_Send(buf.data(), msg_size, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buf.data(), msg_size, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
        }
        t1 = MPI_Wtime();
        return (t1 - t0) / N * 1e6;
    } else if (rank == 1) {
        for (int i = 0; i < N; i++) {
            MPI_Recv(buf.data(), msg_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Send(buf.data(), msg_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    }
    return 0.0;
}
