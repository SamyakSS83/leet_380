#include <mpi.h>

int mpi_ring_sum() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int token = 0;
    int prev = (rank - 1 + size) % size;
    int next = (rank + 1) % size;

    if (rank == 0) {
        token = 0 + rank;  // rank 0 starts
        MPI_Send(&token, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        MPI_Recv(&token, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        return token;
    } else {
        MPI_Recv(&token, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        token += rank;
        MPI_Send(&token, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        return 0;
    }
}
