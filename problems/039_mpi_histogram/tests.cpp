// tests.cpp for problem 039 — MPI Histogram
#include "harness.h"
#include <mpi.h>

std::vector<int> mpi_histogram(const std::vector<int>& local_data, int B);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("each rank contributes rank value", [&]() {
        std::vector<int> data = {rank};
        auto h = mpi_histogram(data, size);
        // All ranks get same global histogram
        ASSERT_EQ((int)h.size(), size);
        for (int r = 0; r < size; r++) ASSERT_EQ(h[r], 1);
    });

    RUN_TEST("all zeros", [&]() {
        std::vector<int> data = {0, 0, 0};
        auto h = mpi_histogram(data, 4);
        ASSERT_EQ(h[0], 3*size);
        for (int b = 1; b < 4; b++) ASSERT_EQ(h[b], 0);
    });

    RUN_TEST("all ranks same data sums", [&]() {
        std::vector<int> data = {0, 1, 2, 3};
        auto h = mpi_histogram(data, 4);
        for (int b = 0; b < 4; b++) ASSERT_EQ(h[b], size);
    });

    RUN_TEST("total equals sum of all local sizes", [&]() {
        std::vector<int> data(5, rank);
        auto h = mpi_histogram(data, 4);
        int total = 0;
        for (int x : h) total += x;
        ASSERT_EQ(total, 5 * size);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
