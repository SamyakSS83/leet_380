// tests.cpp for problem 065 — Hybrid Reduction Custom
#include "harness.h"
#include <mpi.h>
#include <omp.h>
#include <vector>
#include <algorithm>
#include <cfloat>

// MinLoc defined in solve.cpp
MinLoc hybrid_global_min(const std::vector<double>& local_data);

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Helper: each rank gets 'chunk' elements from the full array
    int chunk = 8;

    auto get_local = [&](const std::vector<double>& full) {
        return std::vector<double>(full.begin() + rank * chunk,
                                   full.begin() + rank * chunk + chunk);
    };

    // Test 1: minimum clearly on rank 0
    RUN_TEST("minimum on rank 0", [&]() {
        std::vector<double> full(chunk * size, 5.0);
        full[2] = -10.0; // rank 0, index 2
        auto ml = hybrid_global_min(get_local(full));
        if (rank == 0) {
            ASSERT_APPROX(ml.value, -10.0);
            ASSERT_TRUE(ml.rank == 0, "wrong rank");
            ASSERT_TRUE(ml.local_index == 2, "wrong local index");
        }
    });

    // Test 2: minimum on last rank
    RUN_TEST("minimum on last rank", [&]() {
        std::vector<double> full(chunk * size, 5.0);
        int last_rank = size - 1;
        full[last_rank * chunk + 3] = -99.0;
        auto ml = hybrid_global_min(get_local(full));
        if (rank == 0) {
            ASSERT_APPROX(ml.value, -99.0);
            ASSERT_TRUE(ml.rank == last_rank, "wrong rank");
            ASSERT_TRUE(ml.local_index == 3, "wrong local index");
        }
    });

    // Test 3: all equal values — lower rank wins (rank 0, index 0)
    RUN_TEST("all equal: rank 0 index 0 wins", [&]() {
        std::vector<double> full(chunk * size, 3.14);
        auto ml = hybrid_global_min(get_local(full));
        if (rank == 0) {
            ASSERT_APPROX(ml.value, 3.14);
            ASSERT_TRUE(ml.rank == 0, "rank 0 should win tie");
            ASSERT_TRUE(ml.local_index == 0, "index 0 should win tie");
        }
    });

    // Test 4: random array, value matches std::min of full array
    RUN_TEST("random array: value matches global min", [&]() {
        int total = chunk * size;
        auto full = rand_vec(total, 42);
        auto ml = hybrid_global_min(get_local(full));
        if (rank == 0) {
            double expected_min = *std::min_element(full.begin(), full.end());
            ASSERT_APPROX(ml.value, expected_min);
        }
    });

    // Test 5: non-rank-0 returns default {0,0,0}
    RUN_TEST("non-rank-0 returns default", [&]() {
        auto full = rand_vec(chunk * size, 7);
        auto ml = hybrid_global_min(get_local(full));
        if (rank != 0) {
            ASSERT_APPROX(ml.value, 0.0);
            ASSERT_TRUE(ml.rank == 0, "default rank should be 0");
            ASSERT_TRUE(ml.local_index == 0, "default index should be 0");
        }
    });

    // Test 6: negative values
    RUN_TEST("negative values: correct global min", [&]() {
        std::vector<double> full(chunk * size);
        for (int i = 0; i < chunk * size; i++) full[i] = -(double)(i + 1);
        // min is at last element: -(chunk*size)
        auto ml = hybrid_global_min(get_local(full));
        if (rank == 0) {
            ASSERT_APPROX(ml.value, -(double)(chunk * size));
        }
    });

    // Test 7: rank and local_index are valid indices
    RUN_TEST("rank and local_index are in valid range", [&]() {
        auto full = rand_vec(chunk * size, 55);
        auto ml = hybrid_global_min(get_local(full));
        if (rank == 0) {
            ASSERT_TRUE(ml.rank >= 0 && ml.rank < size, "rank out of range");
            ASSERT_TRUE(ml.local_index >= 0 && ml.local_index < chunk, "local_index out of range");
        }
    });

    // Test 8: minimum at last index of middle rank
    RUN_TEST("minimum at last index of rank 1", [&]() {
        std::vector<double> full(chunk * size, 10.0);
        full[1 * chunk + (chunk - 1)] = -50.0;
        auto ml = hybrid_global_min(get_local(full));
        if (rank == 0) {
            ASSERT_APPROX(ml.value, -50.0);
            ASSERT_TRUE(ml.rank == 1, "wrong rank");
            ASSERT_TRUE(ml.local_index == chunk - 1, "wrong local_index");
        }
    });

    // Test 9: single-element arrays
    RUN_TEST("single-element arrays", [&]() {
        std::vector<double> full(size);
        for (int i = 0; i < size; i++) full[i] = (double)(size - i);
        // full[0] = size, full[1] = size-1, ..., full[size-1] = 1
        // min = 1.0 at rank size-1, local_index 0
        std::vector<double> local = {full[rank]};
        auto ml = hybrid_global_min(local);
        if (rank == 0) {
            ASSERT_APPROX(ml.value, 1.0);
            ASSERT_TRUE(ml.rank == size - 1, "wrong rank for single-elem");
            ASSERT_TRUE(ml.local_index == 0, "wrong local_index for single-elem");
        }
    });

    // Test 10: deterministic across two calls
    RUN_TEST("deterministic: same input same result", [&]() {
        auto full = rand_vec(chunk * size, 333);
        auto ml1 = hybrid_global_min(get_local(full));
        auto ml2 = hybrid_global_min(get_local(full));
        if (rank == 0) {
            ASSERT_APPROX(ml1.value, ml2.value);
            ASSERT_TRUE(ml1.rank == ml2.rank, "rank not deterministic");
            ASSERT_TRUE(ml1.local_index == ml2.local_index, "local_index not deterministic");
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
