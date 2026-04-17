// tests.cpp for problem 064 — Hybrid Floyd-Warshall
#include "harness.h"
#include <mpi.h>
#include <omp.h>
#include <vector>
#include <climits>
#include <algorithm>

std::vector<std::vector<int>> hybrid_floyd_warshall(
    std::vector<std::vector<int>> local_dist, int n);

static const int INF = INT_MAX / 2;

// Serial Floyd-Warshall reference
static std::vector<std::vector<int>> serial_fw(std::vector<std::vector<int>> d) {
    int n = (int)d.size();
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (d[i][k] != INF && d[k][j] != INF)
                    d[i][j] = std::min(d[i][j], d[i][k] + d[k][j]);
    return d;
}

// Distribute rows to local
static std::vector<std::vector<int>> get_local(
    const std::vector<std::vector<int>>& full, int rank, int size) {
    int n = (int)full.size();
    int lr = n / size;
    std::vector<std::vector<int>> local(lr);
    for (int i = 0; i < lr; i++) local[i] = full[rank * lr + i];
    return local;
}

// Build path graph distance matrix (INF = no direct edge)
static std::vector<std::vector<int>> path_dist(int n) {
    std::vector<std::vector<int>> d(n, std::vector<int>(n, INF));
    for (int i = 0; i < n; i++) {
        d[i][i] = 0;
        if (i+1 < n) { d[i][i+1] = 1; d[i+1][i] = 1; }
    }
    return d;
}

// Build complete graph (all edges weight 1)
static std::vector<std::vector<int>> complete_dist(int n) {
    std::vector<std::vector<int>> d(n, std::vector<int>(n, 1));
    for (int i = 0; i < n; i++) d[i][i] = 0;
    return d;
}

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 4 * size;

    auto run = [&](const std::vector<std::vector<int>>& full) {
        auto local = get_local(full, rank, size);
        return hybrid_floyd_warshall(local, n);
    };

    // Test 1: path graph APSP
    RUN_TEST("path graph APSP matches serial", [&]() {
        auto d = path_dist(n);
        auto result = run(d);
        if (rank == 0) {
            auto ref = serial_fw(d);
            ASSERT_TRUE(result == ref, "path graph APSP mismatch");
        }
    });

    // Test 2: complete graph all shortest paths = 1
    RUN_TEST("complete graph all off-diagonal = 1", [&]() {
        auto d = complete_dist(n);
        auto result = run(d);
        if (rank == 0) {
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++) {
                    if (i == j) ASSERT_TRUE(result[i][j] == 0, "self dist not 0");
                    else ASSERT_TRUE(result[i][j] == 1, "off-diag not 1");
                }
        }
    });

    // Test 3: identity matrix (only self-edges weight 0, all others INF)
    RUN_TEST("identity dist matrix stays identity", [&]() {
        std::vector<std::vector<int>> d(n, std::vector<int>(n, INF));
        for (int i = 0; i < n; i++) d[i][i] = 0;
        auto result = run(d);
        if (rank == 0) {
            for (int i = 0; i < n; i++)
                ASSERT_TRUE(result[i][i] == 0, "self dist not 0");
        }
    });

    // Test 4: diagonal is always 0
    RUN_TEST("diagonal always 0 after APSP", [&]() {
        auto d = path_dist(n);
        auto result = run(d);
        if (rank == 0) {
            for (int i = 0; i < n; i++)
                ASSERT_TRUE(result[i][i] == 0, "diagonal not 0");
        }
    });

    // Test 5: symmetry — result[i][j] == result[j][i]
    RUN_TEST("result is symmetric", [&]() {
        auto d = path_dist(n);
        auto result = run(d);
        if (rank == 0) {
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    ASSERT_TRUE(result[i][j] == result[j][i], "not symmetric");
        }
    });

    // Test 6: result size on rank 0 is n x n
    RUN_TEST("result size is n x n on rank 0", [&]() {
        auto d = path_dist(n);
        auto result = run(d);
        if (rank == 0) {
            ASSERT_TRUE((int)result.size() == n, "wrong number of rows");
            for (auto& row : result)
                ASSERT_TRUE((int)row.size() == n, "wrong row length");
        }
    });

    // Test 7: non-rank-0 returns empty
    RUN_TEST("non-rank-0 returns empty", [&]() {
        auto d = path_dist(n);
        auto result = run(d);
        if (rank != 0) ASSERT_TRUE(result.empty(), "non-rank-0 should return empty");
    });

    // Test 8: triangle inequality holds
    RUN_TEST("triangle inequality holds in result", [&]() {
        auto d = path_dist(n);
        auto result = run(d);
        if (rank == 0) {
            // check a subset: result[i][j] <= result[i][k] + result[k][j]
            for (int i = 0; i < n; i += 2)
                for (int j = 0; j < n; j += 2)
                    for (int k = 0; k < n; k += 3) {
                        int rij = result[i][j], rik = result[i][k], rkj = result[k][j];
                        if (rik < INF && rkj < INF)
                            ASSERT_TRUE(rij <= rik + rkj, "triangle inequality violated");
                    }
        }
    });

    // Test 9: weighted path graph matches serial
    RUN_TEST("weighted path graph matches serial", [&]() {
        auto d = path_dist(n);
        // Add some random weights
        for (int i = 0; i+1 < n; i++) {
            int w = (i * 7 + 3) % 5 + 1;
            d[i][i+1] = d[i+1][i] = w;
        }
        auto result = run(d);
        if (rank == 0) {
            auto ref = serial_fw(d);
            ASSERT_TRUE(result == ref, "weighted path mismatch");
        }
    });

    // Test 10: source-to-far-end distance in path graph is n-1
    RUN_TEST("path graph: 0 to n-1 distance is n-1", [&]() {
        auto d = path_dist(n);
        auto result = run(d);
        if (rank == 0)
            ASSERT_TRUE(result[0][n-1] == n-1, "path endpoint distance wrong");
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
