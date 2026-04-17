#include <mpi.h>
#include <omp.h>
#include <vector>

// Hybrid BFS: vertices distributed round-robin across MPI ranks.
// OMP parallelizes frontier expansion per rank.
// Returns full distance array on rank 0, empty on others.
std::vector<int> hybrid_bfs(
    const std::vector<std::vector<int>>& local_adj,
    int total_vertices, int source) {
    // TODO: implement
    return {};
}
