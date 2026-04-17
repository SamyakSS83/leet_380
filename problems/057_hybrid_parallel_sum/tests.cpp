// tests.cpp for problem 057 — Hybrid Parallel Sum
#include "harness.h"
#include <mpi.h>
#include <omp.h>
#include <vector>
#include <numeric>

double hybrid_sum(const std::vector<double>& data);

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("all ones sum = n", [&]() {
        int n = size * 4;
        std::vector<double> data;
        if (rank == 0) data.assign(n, 1.0);
        double result = hybrid_sum(data);
        if (rank == 0) ASSERT_APPROX(result, (double)n);
    });

    RUN_TEST("all zeros", [&]() {
        int n = size * 4;
        std::vector<double> data;
        if (rank == 0) data.assign(n, 0.0);
        double result = hybrid_sum(data);
        if (rank == 0) ASSERT_APPROX(result, 0.0);
    });

    RUN_TEST("sequential 1..n", [&]() {
        int n = size * 4;
        std::vector<double> data;
        double expected = 0;
        if (rank == 0) {
            data.resize(n);
            for (int i = 0; i < n; i++) { data[i] = (double)(i + 1); expected += data[i]; }
        }
        // broadcast expected to all ranks for the check
        MPI_Bcast(&expected, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        double result = hybrid_sum(data);
        if (rank == 0) ASSERT_APPROX(result, expected);
    });

    RUN_TEST("non-rank-0 returns 0.0", [&]() {
        int n = size * 4;
        std::vector<double> data;
        if (rank == 0) data.assign(n, 1.0);
        double result = hybrid_sum(data);
        if (rank != 0) ASSERT_APPROX(result, 0.0);
    });

    RUN_TEST("negative values", [&]() {
        int n = size * 4;
        std::vector<double> data;
        if (rank == 0) data.assign(n, -1.5);
        double result = hybrid_sum(data);
        if (rank == 0) ASSERT_APPROX(result, -1.5 * n);
    });

    RUN_TEST("large array rand sum", [&]() {
        int n = size * 256;
        std::vector<double> data;
        double expected = 0;
        if (rank == 0) {
            data = rand_vec(n, 42, 0.0, 1.0);
            for (double v : data) expected += v;
        }
        MPI_Bcast(&expected, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        double result = hybrid_sum(data);
        if (rank == 0) ASSERT_APPROX(result, expected, 1e-9, 1e-6);
    });

    RUN_TEST("single element per rank", [&]() {
        int n = size;
        std::vector<double> data;
        double expected = 0;
        if (rank == 0) {
            data.resize(n);
            for (int i = 0; i < n; i++) { data[i] = (double)(i * 2); expected += data[i]; }
        }
        MPI_Bcast(&expected, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        double result = hybrid_sum(data);
        if (rank == 0) ASSERT_APPROX(result, expected);
    });

    RUN_TEST("alternating +1 -1 sum near 0", [&]() {
        int n = size * 4;
        std::vector<double> data;
        if (rank == 0) {
            data.resize(n);
            for (int i = 0; i < n; i++) data[i] = (i % 2 == 0) ? 1.0 : -1.0;
        }
        double result = hybrid_sum(data);
        if (rank == 0) ASSERT_APPROX(result, 0.0);
    });

    RUN_TEST("very large values", [&]() {
        int n = size * 4;
        std::vector<double> data;
        if (rank == 0) data.assign(n, 1e8);
        double result = hybrid_sum(data);
        if (rank == 0) ASSERT_APPROX(result, 1e8 * n, 1.0, 1e-6);
    });

    RUN_TEST("mixed large array", [&]() {
        int n = size * 1000;
        std::vector<double> data;
        double expected = 0;
        if (rank == 0) {
            data = rand_vec(n, 99, -100.0, 100.0);
            for (double v : data) expected += v;
        }
        MPI_Bcast(&expected, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        double result = hybrid_sum(data);
        if (rank == 0) ASSERT_APPROX(result, expected, 1e-6, 1e-4);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
