// tests.cpp for problem 032 — MPI Prefix Scan
#include "harness.h"
#include <mpi.h>

double mpi_exclusive_prefix(double local_val);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("exclusive prefix of 1s", [&]() {
        double r = mpi_exclusive_prefix(1.0);
        ASSERT_APPROX(r, (double)rank);  // rank 0->0, rank 1->1, etc
    });

    RUN_TEST("exclusive prefix of rank+1", [&]() {
        double r = mpi_exclusive_prefix((double)(rank+1));
        // rank r gets sum of 1+2+...+r = r*(r+1)/2
        ASSERT_APPROX(r, (double)(rank*(rank+1)/2));
    });

    RUN_TEST("rank 0 always gets 0", [&]() {
        double r = mpi_exclusive_prefix(999.0);
        if (rank == 0) ASSERT_APPROX(r, 0.0);
    });

    RUN_TEST("all zeros gives zero prefix", [&]() {
        double r = mpi_exclusive_prefix(0.0);
        ASSERT_APPROX(r, 0.0);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
