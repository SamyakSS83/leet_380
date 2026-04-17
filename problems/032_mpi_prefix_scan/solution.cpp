#include <mpi.h>

double mpi_exclusive_prefix(double local_val) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double result = 0.0;
    MPI_Exscan(&local_val, &result, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    if (rank == 0) result = 0.0;
    return result;
}
