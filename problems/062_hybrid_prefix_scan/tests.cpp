// tests.cpp for problem 062 — Hybrid Prefix Scan
#include "harness.h"
#include <mpi.h>
#include <omp.h>
#include <vector>
#include <numeric>

std::vector<double> hybrid_prefix_scan(const std::vector<double>& local_data);

// Serial global exclusive prefix sum
static std::vector<double> serial_excl_prefix(const std::vector<double>& v) {
    std::vector<double> out(v.size());
    if (v.empty()) return out;
    out[0] = 0.0;
    for (size_t i = 1; i < v.size(); i++) out[i] = out[i-1] + v[i-1];
    return out;
}

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Helper: distribute full array, run scan, gather and return full result on rank 0
    auto run_gather = [&](const std::vector<double>& full, int chunk) {
        std::vector<double> local(full.begin() + rank * chunk,
                                   full.begin() + rank * chunk + chunk);
        auto local_res = hybrid_prefix_scan(local);

        std::vector<double> gathered;
        if (rank == 0) gathered.resize(full.size());
        MPI_Gather(local_res.data(), chunk, MPI_DOUBLE,
                   gathered.data(), chunk, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        return gathered;
    };

    int chunk = 4; // elements per rank => total = 4*size

    // Test 1: ones array => exclusive prefix is 0,1,2,3,...
    RUN_TEST("ones array gives 0,1,2,...", [&]() {
        int total = chunk * size;
        std::vector<double> full(total, 1.0);
        auto result = run_gather(full, chunk);
        if (rank == 0) {
            auto ref = serial_excl_prefix(full);
            ASSERT_VEC_APPROX(result, ref);
        }
    });

    // Test 2: zeros array => all zeros
    RUN_TEST("zeros array gives all zeros", [&]() {
        int total = chunk * size;
        std::vector<double> full(total, 0.0);
        auto result = run_gather(full, chunk);
        if (rank == 0) {
            for (double v : result) ASSERT_APPROX(v, 0.0);
        }
    });

    // Test 3: 1,2,...,n
    RUN_TEST("sequential values match serial prefix", [&]() {
        int total = chunk * size;
        std::vector<double> full(total);
        for (int i = 0; i < total; i++) full[i] = i + 1;
        auto result = run_gather(full, chunk);
        if (rank == 0) {
            auto ref = serial_excl_prefix(full);
            ASSERT_VEC_APPROX(result, ref);
        }
    });

    // Test 4: random values match serial
    RUN_TEST("random values match serial prefix (seed=42)", [&]() {
        int total = chunk * size;
        auto full = rand_vec(total, 42);
        auto result = run_gather(full, chunk);
        if (rank == 0) {
            auto ref = serial_excl_prefix(full);
            ASSERT_VEC_APPROX(result, ref);
        }
    });

    // Test 5: first element of global result is always 0
    RUN_TEST("first global element is 0", [&]() {
        int total = chunk * size;
        auto full = rand_vec(total, 99);
        auto result = run_gather(full, chunk);
        if (rank == 0) ASSERT_APPROX(result[0], 0.0);
    });

    // Test 6: result size equals input size
    RUN_TEST("local result size equals local input size", [&]() {
        int total = chunk * size;
        auto full = rand_vec(total, 7);
        std::vector<double> local(full.begin() + rank * chunk,
                                   full.begin() + rank * chunk + chunk);
        auto res = hybrid_prefix_scan(local);
        ASSERT_TRUE((int)res.size() == chunk, "local result size mismatch");
    });

    // Test 7: another random seed
    RUN_TEST("random values match serial prefix (seed=123)", [&]() {
        int total = chunk * size;
        auto full = rand_vec(total, 123);
        auto result = run_gather(full, chunk);
        if (rank == 0) {
            auto ref = serial_excl_prefix(full);
            ASSERT_VEC_APPROX(result, ref);
        }
    });

    // Test 8: large chunk
    RUN_TEST("large chunk (100 elems/rank) matches serial", [&]() {
        int big_chunk = 100;
        int total = big_chunk * size;
        auto full = rand_vec(total, 55);
        std::vector<double> local(full.begin() + rank * big_chunk,
                                   full.begin() + rank * big_chunk + big_chunk);
        auto local_res = hybrid_prefix_scan(local);
        std::vector<double> gathered;
        if (rank == 0) gathered.resize(total);
        MPI_Gather(local_res.data(), big_chunk, MPI_DOUBLE,
                   gathered.data(), big_chunk, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            auto ref = serial_excl_prefix(full);
            ASSERT_VEC_APPROX(gathered, ref);
        }
    });

    // Test 9: monotone increasing input
    RUN_TEST("monotone input prefix is strictly increasing", [&]() {
        int total = chunk * size;
        std::vector<double> full(total);
        for (int i = 0; i < total; i++) full[i] = i + 1.0;
        auto result = run_gather(full, chunk);
        if (rank == 0) {
            for (size_t i = 1; i < result.size(); i++)
                ASSERT_TRUE(result[i] > result[i-1], "prefix not strictly increasing");
        }
    });

    // Test 10: last element equals total_sum minus last input element
    RUN_TEST("last prefix element = total_sum - last_elem", [&]() {
        int total = chunk * size;
        auto full = rand_vec(total, 888);
        auto result = run_gather(full, chunk);
        if (rank == 0) {
            double total_sum = 0;
            for (double v : full) total_sum += v;
            double expected_last = total_sum - full.back();
            ASSERT_APPROX(result.back(), expected_last);
        }
    });

    // Test 11: empty local chunk should return empty local output
    RUN_TEST("empty local chunk returns empty output", [&]() {
        std::vector<double> local;
        auto res = hybrid_prefix_scan(local);
        ASSERT_TRUE(res.empty(), "empty local input should produce empty local output");
    });

    // Test 12: one element per rank behaves like global prefix over rank values
    RUN_TEST("single element per rank", [&]() {
        std::vector<double> local = {(double)(rank + 1)};  // 1,2,3,... by rank
        auto res = hybrid_prefix_scan(local);
        ASSERT_TRUE((int)res.size() == 1, "expected one local result");
        double expected = (double)(rank * (rank + 1) / 2);
        ASSERT_APPROX(res[0], expected);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
