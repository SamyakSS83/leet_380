// tests.cpp for problem 034 — MPI Ring Communication
#include "harness.h"
#include <mpi.h>

int mpi_ring_sum();

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int r = mpi_ring_sum();

    RUN_TEST("ring sum equals 0+1+...+(size-1)", [&]() {
        if (rank == 0) {
            int expected = size * (size - 1) / 2;
            ASSERT_EQ(r, expected);
        }
    });

    RUN_TEST("non-root ranks return 0", [&]() {
        if (rank != 0) ASSERT_EQ(r, 0);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
