// tests.cpp for problem 030 — MPI Reduce Sum
#include "harness.h"
#include <mpi.h>

double mpi_distributed_sum(const std::vector<double>& local_data);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("each rank contributes rank+1", [&]() {
        std::vector<double> local = {(double)(rank + 1)};
        double r = mpi_distributed_sum(local);
        if (rank == 0) {
            double expected = size * (size + 1) / 2.0;
            ASSERT_APPROX(r, expected);
        }
    });

    RUN_TEST("all zeros sums to zero", [&]() {
        std::vector<double> local(10, 0.0);
        double r = mpi_distributed_sum(local);
        if (rank == 0) ASSERT_APPROX(r, 0.0);
    });

    RUN_TEST("each rank contributes 1.0", [&]() {
        std::vector<double> local(5, 1.0);
        double r = mpi_distributed_sum(local);
        if (rank == 0) ASSERT_APPROX(r, (double)(5 * size));
    });

    RUN_TEST("non-rank-0 returns 0", [&]() {
        std::vector<double> local = {10.0};
        double r = mpi_distributed_sum(local);
        if (rank != 0) ASSERT_APPROX(r, 0.0);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
