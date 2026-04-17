// tests.cpp for problem 043 — MPI Reduce Scatter
#include "harness.h"
#include <mpi.h>
#include <vector>
#include <cmath>

std::vector<double> reduce_scatter_sum(const std::vector<double>& data, int n);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Test 1: all ones, each segment sums to size
    RUN_TEST("all-ones data", [&]() {
        int n = 5;
        std::vector<double> data(n * size, 1.0);
        auto result = reduce_scatter_sum(data, n);
        ASSERT_EQ((int)result.size(), n);
        for (double v : result) ASSERT_APPROX(v, (double)size);
    });

    // Test 2: only rank 0 contributes, others zero
    RUN_TEST("only rank 0 nonzero", [&]() {
        int n = 4;
        std::vector<double> data(n * size, 0.0);
        if (rank == 0) {
            for (int i = 0; i < n * size; i++) data[i] = (double)(i + 1);
        }
        auto result = reduce_scatter_sum(data, n);
        ASSERT_EQ((int)result.size(), n);
        // rank r's segment: indices [r*n..(r+1)*n), values r*n+1 .. (r+1)*n
        for (int i = 0; i < n; i++) {
            double expected = (double)(rank * n + i + 1);
            ASSERT_APPROX(result[i], expected);
        }
    });

    // Test 3: rank-indexed data: rank r contributes data[i] = r+1 for all i
    RUN_TEST("rank-constant data", [&]() {
        int n = 3;
        std::vector<double> data(n * size, (double)(rank + 1));
        auto result = reduce_scatter_sum(data, n);
        ASSERT_EQ((int)result.size(), n);
        // Sum over all ranks of (rank+1) = size*(size+1)/2
        double expected_sum = (double)(size * (size + 1) / 2);
        for (double v : result) ASSERT_APPROX(v, expected_sum);
    });

    // Test 4: output size correct for n=1
    RUN_TEST("n=1 segment", [&]() {
        int n = 1;
        std::vector<double> data(size, (double)(rank));
        auto result = reduce_scatter_sum(data, n);
        ASSERT_EQ((int)result.size(), 1);
        // rank r gets sum of data[r] across all ranks = sum of r for all ranks? No.
        // data[i] = rank for all i. So element at index r across all ranks is rank for each rank.
        // Sum = 0+1+2+...+(size-1) = size*(size-1)/2
        double expected = (double)(size * (size - 1) / 2);
        ASSERT_APPROX(result[0], expected);
    });

    // Test 5: n=10, random data, verify by manual reduction
    RUN_TEST("n=10 random data correctness", [&]() {
        int n = 10;
        auto data = rand_vec(n * size, 100 + rank);
        auto result = reduce_scatter_sum(data, n);
        ASSERT_EQ((int)result.size(), n);
        // Gather all data to rank 0 and verify
        std::vector<double> all_data(n * size * size);
        MPI_Gather(data.data(), n * size, MPI_DOUBLE,
                   all_data.data(), n * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        // Gather result to rank 0 for full verification
        std::vector<double> all_results(n * size);
        MPI_Gather(result.data(), n, MPI_DOUBLE,
                   all_results.data(), n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            for (int r = 0; r < size; r++) {
                for (int i = 0; i < n; i++) {
                    double expected = 0.0;
                    for (int s = 0; s < size; s++)
                        expected += all_data[s * n * size + r * n + i];
                    ASSERT_TRUE(fabs(all_results[r * n + i] - expected) < 1e-6, "value mismatch");
                }
            }
        }
    });

    // Test 6: zero data returns zeros
    RUN_TEST("zero data", [&]() {
        int n = 6;
        std::vector<double> data(n * size, 0.0);
        auto result = reduce_scatter_sum(data, n);
        for (double v : result) ASSERT_APPROX(v, 0.0);
    });

    // Test 7: negative values
    RUN_TEST("negative values sum", [&]() {
        int n = 4;
        std::vector<double> data(n * size, -(double)(rank + 1));
        auto result = reduce_scatter_sum(data, n);
        double expected = -(double)(size * (size + 1) / 2);
        for (double v : result) ASSERT_APPROX(v, expected);
    });

    // Test 8: large n
    RUN_TEST("large n=500", [&]() {
        int n = 500;
        std::vector<double> data(n * size, 2.0);
        auto result = reduce_scatter_sum(data, n);
        ASSERT_EQ((int)result.size(), n);
        for (double v : result) ASSERT_APPROX(v, 2.0 * size);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
