// tests.cpp for problem 021 — Parallel Floyd-Warshall
#include "harness.h"

// INF defined in solve.cpp
std::vector<double> parallel_floyd_warshall(std::vector<double> dist, int V);

void test_single_node() {
    auto r = parallel_floyd_warshall({0.0}, 1);
    ASSERT_APPROX(r[0], 0.0);
}

void test_two_nodes() {
    std::vector<double> d = {0, 3.0, INF, 0};
    auto r = parallel_floyd_warshall(d, 2);
    ASSERT_APPROX(r[0], 0.0); ASSERT_APPROX(r[1], 3.0);
    ASSERT_APPROX(r[2], INF); ASSERT_APPROX(r[3], 0.0);
}

void test_triangle() {
    int V = 3;
    // 0->1: 1, 1->2: 2, 0->2: 10
    std::vector<double> d = {0,1,10, INF,0,2, INF,INF,0};
    auto r = parallel_floyd_warshall(d, V);
    ASSERT_APPROX(r[0*3+2], 3.0);  // 0->1->2 = 3 < 10
    ASSERT_APPROX(r[0*3+1], 1.0);
    ASSERT_APPROX(r[1*3+2], 2.0);
}

void test_disconnected() {
    int V = 3;
    std::vector<double> d = {0,1,INF, INF,0,INF, INF,INF,0};
    auto r = parallel_floyd_warshall(d, V);
    ASSERT_APPROX(r[0], 0.0);
    ASSERT_APPROX(r[0*3+2], INF);
}

void test_complete_graph() {
    int V = 4;
    std::vector<double> d(V*V, 1.0);
    for (int i = 0; i < V; i++) d[i*V+i] = 0.0;
    auto r = parallel_floyd_warshall(d, V);
    for (int i = 0; i < V; i++)
        for (int j = 0; j < V; j++)
            if (i != j) ASSERT_APPROX(r[i*V+j], 1.0);
}

void test_large() {
    int V = 50;
    std::vector<double> d(V*V, INF);
    for (int i = 0; i < V; i++) {
        d[i*V+i] = 0;
        if (i+1 < V) d[i*V+(i+1)] = 1.0;
    }
    auto r = parallel_floyd_warshall(d, V);
    ASSERT_APPROX(r[0*V+(V-1)], (double)(V-1));
}

int main() {
    RUN_TEST("single node", test_single_node);
    RUN_TEST("two nodes", test_two_nodes);
    RUN_TEST("triangle shortcut", test_triangle);
    RUN_TEST("disconnected", test_disconnected);
    RUN_TEST("complete graph", test_complete_graph);
    RUN_TEST("large path V=50", test_large);
    print_results();
    return 0;
}
