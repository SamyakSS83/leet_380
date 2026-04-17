// tests.cpp for problem 048 — MPI One-Sided RMA Global Max
#include "harness.h"
#include <mpi.h>
#include <cmath>
#include <algorithm>

double rma_global_max(double local_value);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Test 1: max is on rank 0
    RUN_TEST("max on rank 0", [&]() {
        double val = (rank == 0) ? 100.0 : (double)rank;
        double result = rma_global_max(val);
        if (rank == 0) ASSERT_APPROX(result, 100.0);
    });

    // Test 2: max is on last rank
    RUN_TEST("max on last rank", [&]() {
        double val = (rank == size - 1) ? 999.0 : (double)rank;
        double result = rma_global_max(val);
        if (rank == 0) ASSERT_APPROX(result, 999.0);
    });

    // Test 3: all same value
    RUN_TEST("all same value", [&]() {
        double result = rma_global_max(42.0);
        if (rank == 0) ASSERT_APPROX(result, 42.0);
    });

    // Test 4: rank-indexed values, max = size-1
    RUN_TEST("rank as value, max = size-1", [&]() {
        double result = rma_global_max((double)rank);
        if (rank == 0) ASSERT_APPROX(result, (double)(size - 1));
    });

    // Test 5: negative values, max = -1 (rank 0 has -size, rank size-1 has -1)
    RUN_TEST("all negative, max = -1", [&]() {
        double val = -(double)(size - rank);
        double result = rma_global_max(val);
        if (rank == 0) ASSERT_APPROX(result, -1.0);
    });

    // Test 6: non-rank-0 returns their local value
    RUN_TEST("non-rank-0 returns local value", [&]() {
        double val = (double)(rank * 7 + 3);
        double result = rma_global_max(val);
        if (rank != 0) ASSERT_APPROX(result, val);
    });

    // Test 7: max is on rank 2
    RUN_TEST("max on rank 2", [&]() {
        double val = (rank == 2) ? 500.0 : (double)rank;
        double result = rma_global_max(val);
        if (rank == 0) ASSERT_APPROX(result, 500.0);
    });

    // Test 8: floating point precision
    RUN_TEST("floating point values", [&]() {
        double val = (double)rank * 0.1 + 0.01;
        double result = rma_global_max(val);
        if (rank == 0) {
            double expected = (double)(size - 1) * 0.1 + 0.01;
            ASSERT_TRUE(fabs(result - expected) < 1e-9, "floating point max wrong");
        }
    });

    // Test 9: large values
    RUN_TEST("large values", [&]() {
        double val = (double)(rank + 1) * 1e9;
        double result = rma_global_max(val);
        if (rank == 0) ASSERT_APPROX(result, (double)size * 1e9);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
