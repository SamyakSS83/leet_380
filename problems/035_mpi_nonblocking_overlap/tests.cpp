// tests.cpp for problem 035 — MPI Non-blocking Overlap
#include "harness.h"
#include <mpi.h>

std::pair<double, std::vector<std::vector<double>>>
    mpi_nonblocking_collect(const std::vector<double>& local_data);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("local sum of squares correct", [&]() {
        std::vector<double> data = {(double)rank, (double)(rank+1)};
        auto [sos, _all] = mpi_nonblocking_collect(data);
        double expected = (double)rank*(double)rank + (double)(rank+1)*(double)(rank+1);
        ASSERT_APPROX(sos, expected);
    });

    RUN_TEST("rank 0 receives all data", [&]() {
        std::vector<double> data = {(double)rank};
        auto [sos, all] = mpi_nonblocking_collect(data);
        if (rank == 0) {
            ASSERT_EQ((int)all.size(), size);
            for (int r = 0; r < size; r++) ASSERT_APPROX(all[r][0], (double)r);
        }
    });

    RUN_TEST("non-root returns empty all_data", [&]() {
        std::vector<double> data = {1.0, 2.0};
        auto [sos, all] = mpi_nonblocking_collect(data);
        if (rank != 0) ASSERT_TRUE(all.empty(), "non-root should return empty");
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
