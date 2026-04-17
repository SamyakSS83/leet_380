#include <mpi.h>
#include <omp.h>
#include <vector>
#include <climits>

// Hybrid Floyd-Warshall APSP.
// local_dist: this rank's rows of the n×n distance matrix.
// n: total vertices.
// INF = INT_MAX/2 for "no edge".
// Returns full n×n result on rank 0, empty on others.
std::vector<std::vector<int>> hybrid_floyd_warshall(
    std::vector<std::vector<int>> local_dist, int n) {
    // TODO: implement
    return {};
}
