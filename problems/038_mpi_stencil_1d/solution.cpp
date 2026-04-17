#include <mpi.h>
#include <vector>

std::vector<double> mpi_stencil_1d(std::vector<double> local_data, int iters) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int n = (int)local_data.size();

    // Build extended array with ghosts; init boundary ghosts to edge value (Neumann BC)
    std::vector<double> u(n + 2, 0.0);
    std::copy(local_data.begin(), local_data.end(), u.begin() + 1);
    // Boundary ghosts default to edge values for ranks at domain boundary
    if (rank == 0)        u[0]   = u[1];
    if (rank == size - 1) u[n+1] = u[n];
    std::vector<double> tmp(n + 2, 0.0);

    int left  = (rank > 0) ? rank - 1 : MPI_PROC_NULL;
    int right = (rank < size-1) ? rank + 1 : MPI_PROC_NULL;

    for (int iter = 0; iter < iters; iter++) {
        // Exchange ghosts
        MPI_Sendrecv(&u[1], 1, MPI_DOUBLE, left, 0,
                     &u[n+1], 1, MPI_DOUBLE, right, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Sendrecv(&u[n], 1, MPI_DOUBLE, right, 1,
                     &u[0], 1, MPI_DOUBLE, left, 1,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Stencil
        tmp[0] = u[0]; tmp[n+1] = u[n+1];
        for (int i = 1; i <= n; i++)
            tmp[i] = (u[i-1] + u[i] + u[i+1]) / 3.0;
        std::swap(u, tmp);
    }

    return std::vector<double>(u.begin()+1, u.begin()+1+n);
}
