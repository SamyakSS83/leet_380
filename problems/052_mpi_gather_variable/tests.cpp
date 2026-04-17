// tests.cpp for problem 052 — MPI Gather Variable
#include "harness.h"
#include <mpi.h>
#include <vector>
#include <numeric>

std::vector<double> gather_variable(const std::vector<double>& local_data);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("output size on rank 0", [&]() {
        // rank r has r+1 elements, total = size*(size+1)/2
        std::vector<double> local(rank + 1, (double)rank);
        auto result = gather_variable(local);
        if (rank == 0) {
            int expected_size = size * (size + 1) / 2;
            ASSERT_EQ((int)result.size(), expected_size);
        }
    });

    RUN_TEST("non-rank-0 returns empty", [&]() {
        std::vector<double> local(rank + 1, 1.0);
        auto result = gather_variable(local);
        if (rank != 0) ASSERT_TRUE(result.empty(), "should be empty");
    });

    RUN_TEST("rank 0 data is first", [&]() {
        // rank r fills with value (r+1)*1.0 repeated r+1 times
        std::vector<double> local(rank + 1, (double)(rank + 1));
        auto result = gather_variable(local);
        if (rank == 0) {
            // first element from rank 0 = 1.0
            ASSERT_APPROX(result[0], 1.0);
        }
    });

    RUN_TEST("rank 1 data at offset 1", [&]() {
        std::vector<double> local(rank + 1, (double)(rank * 10));
        auto result = gather_variable(local);
        if (rank == 0 && size > 1) {
            // rank 1 contributes 2 elements starting at index 1, each = 10.0
            ASSERT_APPROX(result[1], 10.0);
            ASSERT_APPROX(result[2], 10.0);
        }
    });

    RUN_TEST("rank 2 data at offset 3", [&]() {
        std::vector<double> local(rank + 1, (double)(rank + 100));
        auto result = gather_variable(local);
        if (rank == 0 && size > 2) {
            // rank 2 at offset 0+1+2=3, contributes 3 elements = 102.0
            ASSERT_APPROX(result[3], 102.0);
            ASSERT_APPROX(result[4], 102.0);
            ASSERT_APPROX(result[5], 102.0);
        }
    });

    RUN_TEST("rank 3 data at offset 6", [&]() {
        std::vector<double> local(rank + 1, (double)(rank * rank));
        auto result = gather_variable(local);
        if (rank == 0 && size > 3) {
            // rank 3 at offset 1+2+3=6, contributes 4 elements = 9.0
            ASSERT_APPROX(result[6], 9.0);
        }
    });

    RUN_TEST("unique values per rank", [&]() {
        // rank r: [r*100.0, r*100.0+1, ..., r*100.0+r]
        std::vector<double> local(rank + 1);
        for (int i = 0; i <= rank; i++) local[i] = rank * 100.0 + i;
        auto result = gather_variable(local);
        if (rank == 0) {
            // Check rank 0's contribution: just [0.0]
            ASSERT_APPROX(result[0], 0.0);
        }
    });

    RUN_TEST("all zeros", [&]() {
        std::vector<double> local(rank + 1, 0.0);
        auto result = gather_variable(local);
        if (rank == 0) {
            for (double v : result) ASSERT_APPROX(v, 0.0);
        }
    });

    RUN_TEST("total sum correct", [&]() {
        // rank r contributes (r+1) copies of value (r+1)
        // total sum = sum_{r=0}^{size-1} (r+1)^2
        std::vector<double> local(rank + 1, (double)(rank + 1));
        auto result = gather_variable(local);
        if (rank == 0) {
            double total = 0;
            for (double v : result) total += v;
            double expected = 0;
            for (int r = 0; r < size; r++) expected += (double)(r + 1) * (r + 1);
            ASSERT_APPROX(total, expected);
        }
    });

    RUN_TEST("gather preserves order", [&]() {
        // rank r: [r+0.5]*(r+1)
        std::vector<double> local(rank + 1, rank + 0.5);
        auto result = gather_variable(local);
        if (rank == 0) {
            int idx = 0;
            for (int r = 0; r < size; r++) {
                for (int i = 0; i <= r; i++) {
                    ASSERT_APPROX(result[idx++], r + 0.5);
                }
            }
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
