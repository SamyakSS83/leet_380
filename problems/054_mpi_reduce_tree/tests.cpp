// tests.cpp for problem 054 — MPI Reduce Tree
#include "harness.h"
#include <mpi.h>
#include <cmath>

double tree_reduce_sum(double local_value, int root);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("all ones sum equals size", [&]() {
        double result = tree_reduce_sum(1.0, 0);
        if (rank == 0) ASSERT_APPROX(result, (double)size);
    });

    RUN_TEST("each rank contributes its rank value", [&]() {
        double result = tree_reduce_sum((double)rank, 0);
        double expected = (double)(size * (size - 1)) / 2.0;
        if (rank == 0) ASSERT_APPROX(result, expected);
    });

    RUN_TEST("all zeros", [&]() {
        double result = tree_reduce_sum(0.0, 0);
        if (rank == 0) ASSERT_APPROX(result, 0.0);
    });

    RUN_TEST("reduce to root 1", [&]() {
        double result = tree_reduce_sum(1.0, 1);
        if (rank == 1) ASSERT_APPROX(result, (double)size);
    });

    RUN_TEST("reduce to last rank", [&]() {
        double result = tree_reduce_sum(2.0, size - 1);
        if (rank == size - 1) ASSERT_APPROX(result, 2.0 * size);
    });

    RUN_TEST("negative values", [&]() {
        double result = tree_reduce_sum(-3.5, 0);
        if (rank == 0) ASSERT_APPROX(result, -3.5 * size);
    });

    RUN_TEST("mixed values sum", [&]() {
        // rank r contributes (r % 2 == 0) ? 1.0 : -1.0
        double val = (rank % 2 == 0) ? 1.0 : -1.0;
        double result = tree_reduce_sum(val, 0);
        if (rank == 0) {
            int pos = (size + 1) / 2;
            int neg = size / 2;
            double expected = (double)(pos - neg);
            ASSERT_APPROX(result, expected);
        }
    });

    RUN_TEST("large values", [&]() {
        double result = tree_reduce_sum(1e6, 0);
        if (rank == 0) ASSERT_APPROX(result, 1e6 * size);
    });

    RUN_TEST("fractional values", [&]() {
        double result = tree_reduce_sum(1.0 / size, 0);
        if (rank == 0) ASSERT_APPROX(result, 1.0);
    });

    RUN_TEST("rank-squared sum", [&]() {
        double result = tree_reduce_sum((double)(rank * rank), 0);
        if (rank == 0) {
            double expected = 0;
            for (int r = 0; r < size; r++) expected += r * r;
            ASSERT_APPROX(result, expected);
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
