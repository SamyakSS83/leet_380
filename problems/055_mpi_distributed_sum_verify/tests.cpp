// tests.cpp for problem 055 — MPI Distributed Sum Verify
#include "harness.h"
#include <mpi.h>
#include <vector>
#include <numeric>

bool verify_distributed_sum(const std::vector<double>& full_array);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("all ones returns true", [&]() {
        std::vector<double> arr;
        if (rank == 0) arr.assign(size * 4, 1.0);
        bool ok = verify_distributed_sum(arr);
        ASSERT_TRUE(ok, "all-ones should be consistent");
    });

    RUN_TEST("all zeros returns true", [&]() {
        std::vector<double> arr;
        if (rank == 0) arr.assign(size * 4, 0.0);
        bool ok = verify_distributed_sum(arr);
        ASSERT_TRUE(ok, "all-zeros should be consistent");
    });

    RUN_TEST("sequential values returns true", [&]() {
        std::vector<double> arr;
        if (rank == 0) {
            arr.resize(size * 4);
            for (int i = 0; i < (int)arr.size(); i++) arr[i] = (double)i;
        }
        bool ok = verify_distributed_sum(arr);
        ASSERT_TRUE(ok, "sequential values should be consistent");
    });

    RUN_TEST("negative values returns true", [&]() {
        std::vector<double> arr;
        if (rank == 0) arr.assign(size * 4, -2.5);
        bool ok = verify_distributed_sum(arr);
        ASSERT_TRUE(ok, "negative values should be consistent");
    });

    RUN_TEST("mixed positive and negative", [&]() {
        std::vector<double> arr;
        if (rank == 0) {
            int n = size * 4;
            arr.resize(n);
            for (int i = 0; i < n; i++) arr[i] = (i % 2 == 0) ? 1.0 : -1.0;
        }
        bool ok = verify_distributed_sum(arr);
        ASSERT_TRUE(ok, "mixed values should be consistent");
    });

    RUN_TEST("large array returns true", [&]() {
        std::vector<double> arr;
        if (rank == 0) {
            int n = size * 100;
            arr.resize(n);
            for (int i = 0; i < n; i++) arr[i] = (double)(i % 7) * 0.1;
        }
        bool ok = verify_distributed_sum(arr);
        ASSERT_TRUE(ok, "large array should be consistent");
    });

    RUN_TEST("all ranks return same boolean (true case)", [&]() {
        std::vector<double> arr;
        if (rank == 0) arr.assign(size * 4, 3.14);
        bool ok = verify_distributed_sum(arr);
        // Gather to check all ranks returned true
        int local_int = ok ? 1 : 0;
        int global_sum = 0;
        MPI_Reduce(&local_int, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if (rank == 0) ASSERT_EQ(global_sum, size);
    });

    RUN_TEST("single chunk per rank", [&]() {
        std::vector<double> arr;
        if (rank == 0) {
            arr.resize(size);
            for (int i = 0; i < size; i++) arr[i] = (double)(i + 1);
        }
        bool ok = verify_distributed_sum(arr);
        ASSERT_TRUE(ok, "single-element chunks should be consistent");
    });

    RUN_TEST("powers of two", [&]() {
        std::vector<double> arr;
        if (rank == 0) {
            int n = size * 4;
            arr.resize(n);
            double v = 1.0;
            for (int i = 0; i < n; i++, v *= 2.0) arr[i] = v;
        }
        bool ok = verify_distributed_sum(arr);
        ASSERT_TRUE(ok, "powers of two should be consistent");
    });

    RUN_TEST("constant large value", [&]() {
        std::vector<double> arr;
        if (rank == 0) arr.assign(size * 4, 1e6);
        bool ok = verify_distributed_sum(arr);
        ASSERT_TRUE(ok, "large constant should be consistent");
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
