// tests.cpp for problem 063 — Hybrid BFS
#include "harness.h"
#include <mpi.h>
#include <omp.h>
#include <vector>
#include <queue>
#include <algorithm>

std::vector<int> hybrid_bfs(
    const std::vector<std::vector<int>>& local_adj,
    int total_vertices, int source);

// Serial BFS reference
static std::vector<int> serial_bfs(
    const std::vector<std::vector<int>>& adj, int n, int src) {
    std::vector<int> dist(n, -1);
    std::queue<int> q;
    dist[src] = 0;
    q.push(src);
    while (!q.empty()) {
        int v = q.front(); q.pop();
        for (int nb : adj[v]) {
            if (dist[nb] == -1) {
                dist[nb] = dist[v] + 1;
                q.push(nb);
            }
        }
    }
    return dist;
}

// Build local_adj for a rank given full adjacency list (round-robin distribution)
static std::vector<std::vector<int>> make_local_adj(
    const std::vector<std::vector<int>>& full_adj, int rank, int size) {
    int n = (int)full_adj.size();
    int local_n = 0;
    for (int v = rank; v < n; v += size) local_n++;
    std::vector<std::vector<int>> local(local_n);
    int idx = 0;
    for (int v = rank; v < n; v += size) {
        local[idx++] = full_adj[v];
    }
    return local;
}

// Build a path graph: 0-1-2-...(n-1)
static std::vector<std::vector<int>> path_graph(int n) {
    std::vector<std::vector<int>> adj(n);
    for (int i = 0; i < n-1; i++) {
        adj[i].push_back(i+1);
        adj[i+1].push_back(i);
    }
    return adj;
}

// Build a complete graph Kn
static std::vector<std::vector<int>> complete_graph(int n) {
    std::vector<std::vector<int>> adj(n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (i != j) adj[i].push_back(j);
    return adj;
}

// Build a cycle graph
static std::vector<std::vector<int>> cycle_graph(int n) {
    auto adj = path_graph(n);
    adj[0].push_back(n-1);
    adj[n-1].push_back(0);
    return adj;
}

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    auto run = [&](const std::vector<std::vector<int>>& full_adj, int src) {
        int n = (int)full_adj.size();
        auto local = make_local_adj(full_adj, rank, size);
        return hybrid_bfs(local, n, src);
    };

    // Test 1: path graph source=0
    RUN_TEST("path graph source=0", [&]() {
        int n = 4 * size;
        auto adj = path_graph(n);
        auto result = run(adj, 0);
        if (rank == 0) {
            auto ref = serial_bfs(adj, n, 0);
            ASSERT_VEC_EQ(result, ref);
        }
    });

    // Test 2: path graph source=middle
    RUN_TEST("path graph source=middle", [&]() {
        int n = 4 * size;
        auto adj = path_graph(n);
        int src = n / 2;
        auto result = run(adj, src);
        if (rank == 0) {
            auto ref = serial_bfs(adj, n, src);
            ASSERT_VEC_EQ(result, ref);
        }
    });

    // Test 3: complete graph all distances = 1 (except source = 0)
    RUN_TEST("complete graph all distances 1", [&]() {
        int n = 4 * size;
        auto adj = complete_graph(n);
        auto result = run(adj, 0);
        if (rank == 0) {
            ASSERT_TRUE(result[0] == 0, "source dist should be 0");
            for (int i = 1; i < n; i++)
                ASSERT_TRUE(result[i] == 1, "all others should be dist 1");
        }
    });

    // Test 4: cycle graph source=0
    RUN_TEST("cycle graph source=0", [&]() {
        int n = 4 * size;
        auto adj = cycle_graph(n);
        auto result = run(adj, 0);
        if (rank == 0) {
            auto ref = serial_bfs(adj, n, 0);
            ASSERT_VEC_EQ(result, ref);
        }
    });

    // Test 5: source distance is always 0
    RUN_TEST("source vertex has distance 0", [&]() {
        int n = 4 * size;
        auto adj = path_graph(n);
        int src = 2;
        auto result = run(adj, src);
        if (rank == 0) ASSERT_TRUE(result[src] == 0, "source should have dist 0");
    });

    // Test 6: all vertices reachable (no -1 in result)
    RUN_TEST("all vertices have valid distance", [&]() {
        int n = 4 * size;
        auto adj = cycle_graph(n);
        auto result = run(adj, 0);
        if (rank == 0) {
            for (int d : result)
                ASSERT_TRUE(d >= 0, "unreachable vertex found");
        }
    });

    // Test 7: non-rank-0 returns empty
    RUN_TEST("non-rank-0 returns empty", [&]() {
        int n = 4 * size;
        auto adj = path_graph(n);
        auto result = run(adj, 0);
        if (rank != 0) ASSERT_TRUE(result.empty(), "non-rank-0 should return empty");
    });

    // Test 8: path graph source = last vertex
    RUN_TEST("path graph source=last vertex", [&]() {
        int n = 4 * size;
        auto adj = path_graph(n);
        int src = n - 1;
        auto result = run(adj, src);
        if (rank == 0) {
            auto ref = serial_bfs(adj, n, src);
            ASSERT_VEC_EQ(result, ref);
        }
    });

    // Test 9: result size is total_vertices on rank 0
    RUN_TEST("result size equals total_vertices on rank 0", [&]() {
        int n = 4 * size;
        auto adj = cycle_graph(n);
        auto result = run(adj, 0);
        if (rank == 0) ASSERT_TRUE((int)result.size() == n, "wrong result size");
    });

    // Test 10: cycle graph source=mid matches serial
    RUN_TEST("cycle graph source=mid matches serial", [&]() {
        int n = 4 * size;
        auto adj = cycle_graph(n);
        int src = n / 4;
        auto result = run(adj, src);
        if (rank == 0) {
            auto ref = serial_bfs(adj, n, src);
            ASSERT_VEC_EQ(result, ref);
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
