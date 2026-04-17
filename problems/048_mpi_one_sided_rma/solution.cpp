#include <mpi.h>
#include <climits>
#include <cfloat>

double rma_global_max(double local_value) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Rank 0 exposes a window with its local value as initial max
    double window_val = (rank == 0) ? local_value : 0.0;
    MPI_Win win;
    MPI_Win_create(&window_val, sizeof(double), sizeof(double),
                   MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    MPI_Win_fence(0, win);

    // Every rank (including rank 0 for ranks != 0) accumulates into rank 0
    if (rank != 0) {
        MPI_Accumulate(&local_value, 1, MPI_DOUBLE,
                       0, 0, 1, MPI_DOUBLE, MPI_MAX, win);
    }

    MPI_Win_fence(0, win);
    MPI_Win_free(&win);

    if (rank == 0) return window_val;
    return local_value;
}
