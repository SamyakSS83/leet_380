#include <vector>
#include <algorithm>
#include <omp.h>

static const double INF = 1e18;

std::vector<double> parallel_floyd_warshall(std::vector<double> dist, int V) {
    for (int k = 0; k < V; k++) {
        #pragma omp parallel for collapse(2) schedule(static)
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (dist[i*V+k] < INF && dist[k*V+j] < INF) {
                    double via = dist[i*V+k] + dist[k*V+j];
                    if (via < dist[i*V+j]) dist[i*V+j] = via;
                }
            }
        }
    }
    return dist;
}
