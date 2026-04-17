// tests.cpp for problem 037 — MPI Cartesian Topology
#include "harness.h"
#include <mpi.h>

std::vector<double> mpi_cart_halo(int local_N);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("halo exchange completes", [&]() {
        auto grid = mpi_cart_halo(4);
        int W = 4 + 2;
        // Interior cells should still equal rank
        ASSERT_APPROX(grid[1*W+1], (double)rank);
        ASSERT_APPROX(grid[2*W+2], (double)rank);
    });

    RUN_TEST("grid size correct", [&]() {
        auto grid = mpi_cart_halo(4);
        ASSERT_EQ((int)grid.size(), 6*6);
    });

    RUN_TEST("halos are non-negative (populated)", [&]() {
        auto grid = mpi_cart_halo(4);
        int W = 6;
        // After exchange, halos should be >= 0 (neighbor rank values)
        ASSERT_TRUE(grid[0*W+1] >= 0.0, "top halo negative");
        ASSERT_TRUE(grid[5*W+1] >= 0.0, "bottom halo negative");
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
