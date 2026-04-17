#include <vector>
#include <omp.h>
#include <atomic>

std::vector<int> parallel_bfs(const std::vector<std::vector<int>>& adj, int source) {
    int n = (int)adj.size();
    std::vector<int> dist(n, -1);
    if (n == 0) return dist;
    dist[source] = 0;
    std::vector<int> frontier = {source};

    while (!frontier.empty()) {
        int fsize = (int)frontier.size();
        // Per-thread local next frontiers
        int nthreads = omp_get_max_threads();
        std::vector<std::vector<int>> local_next(nthreads);

        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            #pragma omp for schedule(dynamic, 64) nowait
            for (int fi = 0; fi < fsize; fi++) {
                int u = frontier[fi];
                for (int w : adj[u]) {
                    // Try to claim w
                    #pragma omp critical
                    {
                        if (dist[w] == -1) {
                            dist[w] = dist[u] + 1;
                            local_next[tid].push_back(w);
                        }
                    }
                }
            }
        }

        frontier.clear();
        for (auto& ln : local_next)
            frontier.insert(frontier.end(), ln.begin(), ln.end());
    }
    return dist;
}
