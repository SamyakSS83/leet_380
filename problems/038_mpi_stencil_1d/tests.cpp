// tests.cpp for problem 038 — MPI 1D Stencil
#include "harness.h"
#include <mpi.h>

std::vector<double> mpi_stencil_1d(std::vector<double> local_data, int iters);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    RUN_TEST("zero iterations returns input", [&]() {
        std::vector<double> data = {(double)rank, (double)(rank+1)};
        auto r = mpi_stencil_1d(data, 0);
        ASSERT_APPROX(r[0], (double)rank);
        ASSERT_APPROX(r[1], (double)(rank+1));
    });

    RUN_TEST("uniform data stays uniform", [&]() {
        std::vector<double> data(4, 5.0);
        auto r = mpi_stencil_1d(data, 10);
        for (double x : r) ASSERT_APPROX(x, 5.0);
    });

    RUN_TEST("output size equals input size", [&]() {
        std::vector<double> data = {1.0, 2.0, 3.0, 4.0};
        auto r = mpi_stencil_1d(data, 5);
        ASSERT_EQ((int)r.size(), 4);
    });

    RUN_TEST("values stay in reasonable range", [&]() {
        std::vector<double> data(8, (double)(rank+1));
        auto r = mpi_stencil_1d(data, 20);
        for (double x : r) {
            ASSERT_TRUE(x >= 0.0 && x <= 5.0, "value out of expected range");
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
