#include <mpi.h>
#include <vector>

std::vector<double> scatter_square_gather(std::vector<double> data) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 0;
    if (rank == 0) n = (int)data.size();
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int chunk = n / size;
    std::vector<double> local(chunk);
    MPI_Scatter(rank==0 ? data.data() : nullptr, chunk, MPI_DOUBLE,
                local.data(), chunk, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (auto& x : local) x = x * x;

    std::vector<double> result;
    if (rank == 0) result.resize(n);
    MPI_Gather(local.data(), chunk, MPI_DOUBLE,
               rank==0 ? result.data() : nullptr, chunk, MPI_DOUBLE,
               0, MPI_COMM_WORLD);
    return result;
}
