#include <mpi.h>
#include <vector>

// Distributed BFS from `source`. Vertex v owned by rank v%size.
// local_adj[i] = neighbors of vertex i*size+rank.
// Returns distance array (total_vertices) on rank 0, empty on others.
std::vector<int> distributed_bfs(
    const std::vector<std::vector<int>>& local_adj,
    int total_vertices,
    int source) {
    // TODO: implement
    return {};
}
