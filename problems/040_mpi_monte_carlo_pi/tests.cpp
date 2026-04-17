// tests.cpp for problem 040 — MPI Monte Carlo Pi
#include "harness.h"
#include <mpi.h>
#include <cmath>

double mpi_monte_carlo_pi(long long n_local, unsigned base_seed);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    RUN_TEST("pi estimate close to 3.14159 (1M total)", [&]() {
        double pi = mpi_monte_carlo_pi(250000, 42);
        if (rank == 0) ASSERT_TRUE(fabs(pi - 3.14159) < 0.05, "pi estimate too far off");
    });

    RUN_TEST("non-rank-0 returns 0", [&]() {
        double pi = mpi_monte_carlo_pi(1000, 1);
        if (rank != 0) ASSERT_APPROX(pi, 0.0);
    });

    RUN_TEST("result is in [0, 4]", [&]() {
        double pi = mpi_monte_carlo_pi(10, 99);
        if (rank == 0) ASSERT_TRUE(pi >= 0.0 && pi <= 4.0, "result out of range");
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
