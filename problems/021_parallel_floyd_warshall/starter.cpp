#include <vector>
#include <omp.h>

static const double INF = 1e18;

// Floyd-Warshall APSP. dist is VxV row-major matrix.
// dist[i*V+j] = edge weight from i to j, INF if no edge, 0 if i==j.
// Return updated shortest-path matrix.
std::vector<double> parallel_floyd_warshall(std::vector<double> dist, int V) {
    // TODO: implement - parallelize i,j loops for each k
    return dist;
}
