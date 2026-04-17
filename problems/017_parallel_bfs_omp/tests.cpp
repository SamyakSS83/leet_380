// tests.cpp for problem 017 — Parallel BFS (OpenMP)
#include "harness.h"

std::vector<int> parallel_bfs(const std::vector<std::vector<int>>& adj, int source);

// Build undirected adjacency list
static std::vector<std::vector<int>> make_graph(int n,
    std::vector<std::pair<int,int>> edges) {
    std::vector<std::vector<int>> adj(n);
    for (auto [u,v] : edges) { adj[u].push_back(v); adj[v].push_back(u); }
    return adj;
}

void test_single_node() {
    auto adj = make_graph(1, {});
    auto d = parallel_bfs(adj, 0);
    ASSERT_EQ(d[0], 0);
}

void test_path() {
    auto adj = make_graph(5, {{0,1},{1,2},{2,3},{3,4}});
    auto d = parallel_bfs(adj, 0);
    std::vector<int> exp = {0,1,2,3,4};
    ASSERT_VEC_EQ(d, exp);
}

void test_disconnected() {
    auto adj = make_graph(4, {{0,1},{2,3}});
    auto d = parallel_bfs(adj, 0);
    ASSERT_EQ(d[0], 0); ASSERT_EQ(d[1], 1);
    ASSERT_EQ(d[2], -1); ASSERT_EQ(d[3], -1);
}

void test_star() {
    auto adj = make_graph(5, {{0,1},{0,2},{0,3},{0,4}});
    auto d = parallel_bfs(adj, 0);
    ASSERT_EQ(d[0], 0);
    for (int i = 1; i < 5; i++) ASSERT_EQ(d[i], 1);
}

void test_cycle() {
    auto adj = make_graph(4, {{0,1},{1,2},{2,3},{3,0}});
    auto d = parallel_bfs(adj, 0);
    ASSERT_EQ(d[0], 0); ASSERT_EQ(d[1], 1);
    ASSERT_EQ(d[2], 2); ASSERT_EQ(d[3], 1);
}

void test_source_not_zero() {
    auto adj = make_graph(5, {{0,1},{1,2},{2,3},{3,4}});
    auto d = parallel_bfs(adj, 2);
    ASSERT_EQ(d[2], 0); ASSERT_EQ(d[0], 2); ASSERT_EQ(d[4], 2);
}

void test_large_path() {
    int n = 1000;
    std::vector<std::pair<int,int>> edges;
    for (int i = 0; i < n-1; i++) edges.push_back({i, i+1});
    auto adj = make_graph(n, edges);
    auto d = parallel_bfs(adj, 0);
    for (int i = 0; i < n; i++) ASSERT_EQ(d[i], i);
}

int main() {
    RUN_TEST("single node", test_single_node);
    RUN_TEST("path graph", test_path);
    RUN_TEST("disconnected", test_disconnected);
    RUN_TEST("star graph", test_star);
    RUN_TEST("cycle graph", test_cycle);
    RUN_TEST("source not zero", test_source_not_zero);
    RUN_TEST("large path 1000", test_large_path);
    print_results();
    return 0;
}
