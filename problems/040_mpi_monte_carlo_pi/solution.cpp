#include <mpi.h>
#include <cstdlib>

double mpi_monte_carlo_pi(long long n_local, unsigned base_seed) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    unsigned seed = base_seed + (unsigned)rank * 1000003u;
    long long inside = 0;
    for (long long i = 0; i < n_local; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;
        if (x*x + y*y < 1.0) inside++;
    }

    long long total_inside = 0;
    MPI_Reduce(&inside, &total_inside, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
        return 4.0 * (double)total_inside / (double)(n_local * size);
    return 0.0;
}
