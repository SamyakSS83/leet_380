// tests.cpp for problem 041 — MPI Gather Stats
#include "harness.h"
#include <mpi.h>
#include <vector>
#include <cmath>

// Stats defined in solve.cpp
Stats gather_stats(const std::vector<double>& local_data);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Test 1: uniform data [1..4] one per rank
    RUN_TEST("uniform single element per rank", [&]() {
        std::vector<double> local = {(double)(rank + 1)};
        Stats s = gather_stats(local);
        if (rank == 0) {
            ASSERT_APPROX(s.min, 1.0);
            ASSERT_APPROX(s.max, (double)size);
            ASSERT_APPROX(s.sum, (double)(size * (size + 1) / 2));
            ASSERT_EQ(s.count, (long long)size);
        }
    });

    // Test 2: fixed arrays of length 3 per rank
    RUN_TEST("fixed arrays length 3", [&]() {
        // rank r gets {r*3+1, r*3+2, r*3+3}
        std::vector<double> local = {(double)(rank*3+1), (double)(rank*3+2), (double)(rank*3+3)};
        Stats s = gather_stats(local);
        if (rank == 0) {
            ASSERT_APPROX(s.min, 1.0);
            ASSERT_APPROX(s.max, (double)(size*3));
            long long total = (long long)(size * 3);
            double expected_sum = total * (total + 1) / 2.0;
            ASSERT_APPROX(s.sum, expected_sum);
            ASSERT_EQ(s.count, (long long)(size * 3));
        }
    });

    // Test 3: negative values
    RUN_TEST("negative values", [&]() {
        std::vector<double> local = {-(double)(rank+1), (double)(rank+1)};
        Stats s = gather_stats(local);
        if (rank == 0) {
            ASSERT_APPROX(s.min, -(double)size);
            ASSERT_APPROX(s.max, (double)size);
            ASSERT_APPROX(s.sum, 0.0);
            ASSERT_EQ(s.count, (long long)(size * 2));
        }
    });

    // Test 4: rank 0 gets empty array, others have data
    RUN_TEST("rank 0 empty array", [&]() {
        std::vector<double> local = (rank == 0) ? std::vector<double>{} : std::vector<double>{(double)rank};
        Stats s = gather_stats(local);
        if (rank == 0) {
            ASSERT_APPROX(s.min, 1.0);
            ASSERT_APPROX(s.max, (double)(size - 1));
            ASSERT_EQ(s.count, (long long)(size - 1));
        }
    });

    // Test 5: all ranks return 0 on non-rank-0
    RUN_TEST("non-rank-0 returns zeroed stats", [&]() {
        std::vector<double> local = {1.0, 2.0, 3.0};
        Stats s = gather_stats(local);
        if (rank != 0) {
            ASSERT_APPROX(s.min, 0.0);
            ASSERT_APPROX(s.max, 0.0);
            ASSERT_APPROX(s.sum, 0.0);
            ASSERT_EQ(s.count, 0LL);
        }
    });

    // Test 6: large arrays with seed-based data
    RUN_TEST("random large arrays", [&]() {
        auto v = rand_vec(1000, 42 + rank);
        Stats s = gather_stats(v);
        if (rank == 0) {
            ASSERT_TRUE(s.count == (long long)(size * 1000), "count mismatch");
            ASSERT_TRUE(s.min <= s.max, "min > max");
            ASSERT_TRUE(s.sum != 0.0, "sum should be nonzero");
        }
    });

    // Test 7: single rank gets all, count consistency
    RUN_TEST("count equals total elements", [&]() {
        std::vector<double> local(rank + 2, (double)rank);  // rank r has r+2 elements
        Stats s = gather_stats(local);
        if (rank == 0) {
            long long expected_count = 0;
            for (int r = 0; r < size; r++) expected_count += (r + 2);
            ASSERT_EQ(s.count, expected_count);
        }
    });

    // Test 8: all same value
    RUN_TEST("all same value", [&]() {
        std::vector<double> local(5, 7.0);
        Stats s = gather_stats(local);
        if (rank == 0) {
            ASSERT_APPROX(s.min, 7.0);
            ASSERT_APPROX(s.max, 7.0);
            ASSERT_APPROX(s.sum, 7.0 * size * 5);
            ASSERT_EQ(s.count, (long long)(size * 5));
        }
    });

    // Test 9: all ranks empty => rank 0 should get zeroed stats
    RUN_TEST("all ranks empty", [&]() {
        std::vector<double> local;
        Stats s = gather_stats(local);
        if (rank == 0) {
            ASSERT_APPROX(s.min, 0.0);
            ASSERT_APPROX(s.max, 0.0);
            ASSERT_APPROX(s.sum, 0.0);
            ASSERT_EQ(s.count, 0LL);
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
