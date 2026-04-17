// tests.cpp for problem 045 — MPI Distributed BFS
#include "harness.h"
#include <mpi.h>
#include <vector>

std::vector<int> distributed_bfs(
    const std::vector<std::vector<int>>& local_adj,
    int total_vertices, int source);

// Build local_adj for a given rank from a full adjacency list
static std::vector<std::vector<int>> make_local_adj(
    const std::vector<std::vector<int>>& full_adj, int rank, int size) {
    int total = (int)full_adj.size();
    std::vector<std::vector<int>> local;
    for (int v = rank; v < total; v += size)
        local.push_back(full_adj[v]);
    return local;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Test 1: linear chain 0-1-2-3-4-5-6-7
    RUN_TEST("linear chain distances", [&]() {
        int V = 8;
        std::vector<std::vector<int>> full(V);
        for (int i = 0; i + 1 < V; i++) { full[i].push_back(i+1); full[i+1].push_back(i); }
        auto local = make_local_adj(full, rank, size);
        auto dist = distributed_bfs(local, V, 0);
        if (rank == 0) {
            ASSERT_EQ((int)dist.size(), V);
            for (int i = 0; i < V; i++) ASSERT_EQ(dist[i], i);
        }
    });

    // Test 2: complete graph K8, all distances = 1 except source = 0
    RUN_TEST("complete graph all dist 1", [&]() {
        int V = 8;
        std::vector<std::vector<int>> full(V);
        for (int i = 0; i < V; i++)
            for (int j = 0; j < V; j++)
                if (i != j) full[i].push_back(j);
        auto local = make_local_adj(full, rank, size);
        auto dist = distributed_bfs(local, V, 0);
        if (rank == 0) {
            ASSERT_EQ(dist[0], 0);
            for (int i = 1; i < V; i++) ASSERT_EQ(dist[i], 1);
        }
    });

    // Test 3: disconnected graph — vertices 4-7 unreachable from 0
    RUN_TEST("disconnected graph unreachable", [&]() {
        int V = 8;
        std::vector<std::vector<int>> full(V);
        // Component 1: 0-1-2-3
        full[0].push_back(1); full[1].push_back(0);
        full[1].push_back(2); full[2].push_back(1);
        full[2].push_back(3); full[3].push_back(2);
        // Component 2: 4-5-6-7 (disconnected)
        full[4].push_back(5); full[5].push_back(4);
        full[5].push_back(6); full[6].push_back(5);
        full[6].push_back(7); full[7].push_back(6);
        auto local = make_local_adj(full, rank, size);
        auto dist = distributed_bfs(local, V, 0);
        if (rank == 0) {
            ASSERT_EQ(dist[0], 0); ASSERT_EQ(dist[1], 1); ASSERT_EQ(dist[2], 2); ASSERT_EQ(dist[3], 3);
            ASSERT_EQ(dist[4], -1); ASSERT_EQ(dist[5], -1); ASSERT_EQ(dist[6], -1); ASSERT_EQ(dist[7], -1);
        }
    });

    // Test 4: star graph, center = 0
    RUN_TEST("star graph from center", [&]() {
        int V = 8;
        std::vector<std::vector<int>> full(V);
        for (int i = 1; i < V; i++) { full[0].push_back(i); full[i].push_back(0); }
        auto local = make_local_adj(full, rank, size);
        auto dist = distributed_bfs(local, V, 0);
        if (rank == 0) {
            ASSERT_EQ(dist[0], 0);
            for (int i = 1; i < V; i++) ASSERT_EQ(dist[i], 1);
        }
    });

    // Test 5: source = isolated single vertex
    RUN_TEST("only source reachable", [&]() {
        int V = 4;
        std::vector<std::vector<int>> full(V); // no edges
        auto local = make_local_adj(full, rank, size);
        auto dist = distributed_bfs(local, V, 0);
        if (rank == 0) {
            ASSERT_EQ(dist[0], 0);
            for (int i = 1; i < V; i++) ASSERT_EQ(dist[i], -1);
        }
    });

    // Test 6: cycle of 8 vertices
    RUN_TEST("8-cycle BFS distances", [&]() {
        int V = 8;
        std::vector<std::vector<int>> full(V);
        for (int i = 0; i < V; i++) {
            full[i].push_back((i+1) % V);
            full[i].push_back((i-1+V) % V);
        }
        auto local = make_local_adj(full, rank, size);
        auto dist = distributed_bfs(local, V, 0);
        if (rank == 0) {
            // In cycle: dist[i] = min(i, V-i)
            for (int i = 0; i < V; i++) ASSERT_EQ(dist[i], std::min(i, V-i));
        }
    });

    // Test 7: BFS from non-zero source
    RUN_TEST("BFS from source=3 on chain", [&]() {
        int V = 8;
        std::vector<std::vector<int>> full(V);
        for (int i = 0; i + 1 < V; i++) { full[i].push_back(i+1); full[i+1].push_back(i); }
        auto local = make_local_adj(full, rank, size);
        auto dist = distributed_bfs(local, V, 3);
        if (rank == 0) {
            for (int i = 0; i < V; i++) ASSERT_EQ(dist[i], std::abs(i - 3));
        }
    });

    // Test 8: two-hop graph
    RUN_TEST("two-hop graph", [&]() {
        int V = 8;
        // 0 -> 1,2; 1 -> 3,4; 2 -> 5,6; 3 -> 7
        std::vector<std::vector<int>> full(V);
        auto addedge = [&](int u, int v){ full[u].push_back(v); full[v].push_back(u); };
        addedge(0,1); addedge(0,2); addedge(1,3); addedge(1,4); addedge(2,5); addedge(2,6); addedge(3,7);
        auto local = make_local_adj(full, rank, size);
        auto dist = distributed_bfs(local, V, 0);
        if (rank == 0) {
            std::vector<int> expected = {0,1,1,2,2,2,2,3};
            ASSERT_VEC_EQ(dist, expected);
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
