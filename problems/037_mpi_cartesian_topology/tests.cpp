// tests.cpp for problem 037 — MPI Cartesian Topology
#include "harness.h"
#include <mpi.h>

std::vector<double> mpi_cart_halo(int local_N);

static void expected_neighbors(int& up, int& down, int& left, int& right) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int dims[2] = {0, 0};
    MPI_Dims_create(size, 2, dims);
    int periods[2] = {1, 1};
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

    MPI_Cart_shift(cart_comm, 0, 1, &up, &down);
    MPI_Cart_shift(cart_comm, 1, 1, &left, &right);
    MPI_Comm_free(&cart_comm);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("interior remains local rank", [&]() {
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

    RUN_TEST("halo rows and cols match cartesian neighbors", [&]() {
        auto grid = mpi_cart_halo(4);
        int W = 6;
        int up, down, left, right;
        expected_neighbors(up, down, left, right);

        for (int j = 1; j <= 4; j++) {
            ASSERT_APPROX(grid[0 * W + j], (double)up);
            ASSERT_APPROX(grid[5 * W + j], (double)down);
        }
        for (int i = 1; i <= 4; i++) {
            ASSERT_APPROX(grid[i * W + 0], (double)left);
            ASSERT_APPROX(grid[i * W + 5], (double)right);
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
