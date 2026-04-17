#include <vector>
#include <omp.h>

// BFS from source on undirected graph (adjacency list).
// Return distances[] where distances[v] = shortest path length, -1 if unreachable.
std::vector<int> parallel_bfs(const std::vector<std::vector<int>>& adj, int source) {
    // TODO: implement level-synchronous BFS with OMP parallelism
    return {};
}
