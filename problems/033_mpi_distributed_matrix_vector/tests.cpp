// tests.cpp for problem 033 — MPI Distributed MatVec
#include "harness.h"
#include <mpi.h>

std::vector<double> mpi_matvec(
    const std::vector<double>& local_rows_flat,
    const std::vector<double>& x_full,
    int local_nrows, int ncols);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("identity times ones", [&]() {
        int ncols = size;
        int local_nrows = 1;
        // Identity matrix: rank r has row r with 1 at position r
        std::vector<double> local_row(ncols, 0.0);
        local_row[rank] = 1.0;
        std::vector<double> x_full;
        if (rank == 0) x_full.assign(ncols, 1.0);
        auto y = mpi_matvec(local_row, x_full, local_nrows, ncols);
        if (rank == 0)
            for (int i = 0; i < size; i++) ASSERT_APPROX(y[i], 1.0);
    });

    RUN_TEST("all-ones matrix times all-ones vector", [&]() {
        int ncols = 4;
        std::vector<double> local_row(ncols, 1.0);
        std::vector<double> x_full;
        if (rank == 0) x_full.assign(ncols, 1.0);
        auto y = mpi_matvec(local_row, x_full, 1, ncols);
        if (rank == 0)
            for (int i = 0; i < size; i++) ASSERT_APPROX(y[i], (double)ncols);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
