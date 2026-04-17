#include <mpi.h>
#include <vector>

std::vector<double> mpi_matvec(
    const std::vector<double>& local_rows_flat,
    const std::vector<double>& x_full,
    int local_nrows, int ncols) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Broadcast x
    std::vector<double> x(ncols);
    if (rank == 0) x = x_full;
    MPI_Bcast(x.data(), ncols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Local matvec
    std::vector<double> local_y(local_nrows, 0.0);
    for (int i = 0; i < local_nrows; i++)
        for (int j = 0; j < ncols; j++)
            local_y[i] += local_rows_flat[i*ncols+j] * x[j];

    // Gather
    int total_rows = local_nrows * size;
    std::vector<double> result;
    if (rank == 0) result.resize(total_rows);
    MPI_Gather(local_y.data(), local_nrows, MPI_DOUBLE,
               rank==0 ? result.data() : nullptr, local_nrows, MPI_DOUBLE,
               0, MPI_COMM_WORLD);
    return result;
}
