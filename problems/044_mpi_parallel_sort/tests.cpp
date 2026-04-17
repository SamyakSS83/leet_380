// tests.cpp for problem 044 — MPI Parallel Sort
#include "harness.h"
#include <mpi.h>
#include <vector>
#include <algorithm>
#include <climits>
#include <numeric>

std::vector<int> parallel_sample_sort(std::vector<int> local_data);

// Gather all sorted partitions to rank 0, verify they form a globally sorted array
static std::vector<int> gather_all(const std::vector<int>& local, int rank, int size) {
    int local_n = (int)local.size();
    std::vector<int> counts(size), displs(size);
    MPI_Gather(&local_n, 1, MPI_INT, counts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        displs[0] = 0;
        for (int i = 1; i < size; i++) displs[i] = displs[i-1] + counts[i-1];
    }
    int total = 0;
    if (rank == 0) for (int c : counts) total += c;
    std::vector<int> all(total);
    MPI_Gatherv(local.data(), local_n, MPI_INT, all.data(), counts.data(), displs.data(), MPI_INT, 0, MPI_COMM_WORLD);
    return all;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Test 1: sorted 1..16 distributed 4 per rank
    RUN_TEST("already sorted input", [&]() {
        std::vector<int> local(4);
        std::iota(local.begin(), local.end(), rank * 4 + 1);
        auto result = parallel_sample_sort(local);
        auto all = gather_all(result, rank, size);
        if (rank == 0) {
            ASSERT_EQ((int)all.size(), 16);
            ASSERT_TRUE(std::is_sorted(all.begin(), all.end()), "not sorted");
        }
    });

    // Test 2: reverse sorted
    RUN_TEST("reverse sorted input", [&]() {
        std::vector<int> local(4);
        for (int i = 0; i < 4; i++) local[i] = 16 - (rank * 4 + i);
        auto result = parallel_sample_sort(local);
        auto all = gather_all(result, rank, size);
        if (rank == 0) {
            ASSERT_TRUE(std::is_sorted(all.begin(), all.end()), "not sorted");
        }
    });

    // Test 3: all same values
    RUN_TEST("all same values", [&]() {
        std::vector<int> local(5, 42);
        auto result = parallel_sample_sort(local);
        auto all = gather_all(result, rank, size);
        if (rank == 0) {
            ASSERT_EQ((int)all.size(), size * 5);
            ASSERT_TRUE(std::is_sorted(all.begin(), all.end()), "not sorted");
            for (int v : all) ASSERT_EQ(v, 42);
        }
    });

    // Test 4: random data, verify globally sorted and no elements lost
    RUN_TEST("random data globally sorted", [&]() {
        auto v = rand_vec_i(100, 7 + rank * 31);
        auto result = parallel_sample_sort(v);
        // local portion must be sorted
        ASSERT_TRUE(std::is_sorted(result.begin(), result.end()), "local not sorted");
        auto all = gather_all(result, rank, size);
        if (rank == 0) {
            ASSERT_EQ((int)all.size(), size * 100);
            ASSERT_TRUE(std::is_sorted(all.begin(), all.end()), "globally not sorted");
        }
    });

    // Test 5: verify no elements are lost (multiset equality)
    RUN_TEST("no elements lost", [&]() {
        auto v = rand_vec_i(50, 99 + rank);
        std::vector<int> original = v;
        auto result = parallel_sample_sort(v);
        auto all = gather_all(result, rank, size);
        // gather originals
        auto all_orig = gather_all(original, rank, size);
        if (rank == 0) {
            std::sort(all_orig.begin(), all_orig.end());
            ASSERT_VEC_EQ(all, all_orig);
        }
    });

    // Test 6: single element per rank
    RUN_TEST("single element per rank", [&]() {
        std::vector<int> local = {size - rank};  // rank 0 gets size, rank size-1 gets 1
        auto result = parallel_sample_sort(local);
        auto all = gather_all(result, rank, size);
        if (rank == 0) {
            ASSERT_EQ((int)all.size(), size);
            ASSERT_TRUE(std::is_sorted(all.begin(), all.end()), "not sorted");
        }
    });

    // Test 7: negative numbers
    RUN_TEST("negative numbers", [&]() {
        std::vector<int> local(4);
        for (int i = 0; i < 4; i++) local[i] = -(rank * 4 + i + 1);
        auto result = parallel_sample_sort(local);
        ASSERT_TRUE(std::is_sorted(result.begin(), result.end()), "local not sorted");
        auto all = gather_all(result, rank, size);
        if (rank == 0) ASSERT_TRUE(std::is_sorted(all.begin(), all.end()), "globally not sorted");
    });

    // Test 8: rank ordering — rank 0's max <= rank 1's min
    RUN_TEST("rank ordering property", [&]() {
        auto v = rand_vec_i(200, 17 + rank * 7);
        auto result = parallel_sample_sort(v);
        // Gather local max/min from adjacent ranks
        int local_min = result.empty() ? INT_MAX : result.front();
        int local_max = result.empty() ? INT_MIN : result.back();
        std::vector<int> all_min(size), all_max(size);
        MPI_Gather(&local_min, 1, MPI_INT, all_min.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Gather(&local_max, 1, MPI_INT, all_max.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            for (int r = 0; r + 1 < size; r++) {
                if (!result.empty())
                    ASSERT_TRUE(all_max[r] <= all_min[r+1], "rank ordering violated");
            }
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
