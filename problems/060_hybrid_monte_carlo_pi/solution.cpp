#include <mpi.h>
#include <omp.h>
#include <cstdlib>
#include <cmath>

double hybrid_monte_carlo_pi(long long samples_per_thread) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int max_threads = omp_get_max_threads();
    long long local_inside = 0;

    #pragma omp parallel reduction(+:local_inside)
    {
        int tid = omp_get_thread_num();
        unsigned seed = (unsigned)(rank * max_threads + tid) * 1000003u + 1u;
        long long inside = 0;
        for (long long i = 0; i < samples_per_thread; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;
            if (x*x + y*y < 1.0) inside++;
        }
        local_inside += inside;
    }

    long long total_inside = 0;
    MPI_Reduce(&local_inside, &total_inside, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        long long total_samples = (long long)samples_per_thread * max_threads * size;
        return 4.0 * (double)total_inside / (double)total_samples;
    }
    return 0.0;
}
