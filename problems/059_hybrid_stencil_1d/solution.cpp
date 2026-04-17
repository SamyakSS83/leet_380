#include <mpi.h>
#include <omp.h>
#include <vector>

std::vector<double> hybrid_stencil_1d(
    std::vector<double> local_u, int iterations) {

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_size = (int)local_u.size(); // includes 2 ghost cells
    int interior = local_size - 2;

    std::vector<double> new_u(local_size);

    for (int iter = 0; iter < iterations; iter++) {
        // Exchange ghost cells with neighbors
        MPI_Request reqs[4];
        int nreq = 0;

        // Send left interior to left neighbor's right ghost
        if (rank > 0)
            MPI_Isend(&local_u[1], 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &reqs[nreq++]);
        // Send right interior to right neighbor's left ghost
        if (rank < size-1)
            MPI_Isend(&local_u[interior], 1, MPI_DOUBLE, rank+1, 1, MPI_COMM_WORLD, &reqs[nreq++]);
        // Receive left ghost from left neighbor
        if (rank > 0)
            MPI_Irecv(&local_u[0], 1, MPI_DOUBLE, rank-1, 1, MPI_COMM_WORLD, &reqs[nreq++]);
        // Receive right ghost from right neighbor
        if (rank < size-1)
            MPI_Irecv(&local_u[local_size-1], 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &reqs[nreq++]);

        MPI_Waitall(nreq, reqs, MPI_STATUSES_IGNORE);

        // Apply stencil to interior using OMP
        #pragma omp parallel for schedule(static)
        for (int i = 1; i <= interior; i++) {
            new_u[i] = 0.5 * (local_u[i-1] + local_u[i+1]);
        }
        // Keep ghost values fixed
        new_u[0] = local_u[0];
        new_u[local_size-1] = local_u[local_size-1];

        std::swap(local_u, new_u);
    }

    // Gather interior from all ranks to rank 0
    std::vector<double> interior_data(local_u.begin()+1, local_u.begin()+1+interior);
    std::vector<double> full_result;

    if (rank == 0) full_result.resize(interior * size);

    MPI_Gather(interior_data.data(), interior, MPI_DOUBLE,
               full_result.data(), interior, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    return full_result;
}
