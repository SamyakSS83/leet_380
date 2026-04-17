#include <mpi.h>
#include <omp.h>
#include <vector>
#include <algorithm>

std::vector<int> hybrid_bfs(
    const std::vector<std::vector<int>>& local_adj,
    int total_vertices, int source) {

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Global distance array (replicated on all ranks, -1 = unvisited)
    std::vector<int> dist(total_vertices, -1);
    dist[source] = 0;

    // Current frontier: local vertices in current BFS level
    std::vector<int> frontier;
    // If this rank owns source
    if (source % size == rank) frontier.push_back(source);

    bool global_active = true;
    while (global_active) {
        // Each rank expands its local frontier using OMP
        int nthreads = omp_get_max_threads();
        std::vector<std::vector<int>> thread_new(nthreads);

        #pragma omp parallel
        {
            int tid = omp_get_thread_num();
            #pragma omp for schedule(dynamic, 1)
            for (int fi = 0; fi < (int)frontier.size(); fi++) {
                int v = frontier[fi];
                int local_idx = v / size; // local_adj[local_idx]
                for (int nb : local_adj[local_idx]) {
                    if (dist[nb] == -1) {
                        // Tentatively set (may race — we'll allreduce)
                        thread_new[tid].push_back(nb);
                    }
                }
            }
        }

        // Merge new candidates (may have duplicates)
        std::vector<int> new_verts;
        for (auto& tv : thread_new)
            for (int v : tv)
                if (dist[v] == -1)
                    new_verts.push_back(v);

        // Build local update array: new_dist[v] = proposed_level, else -1
        int cur_level = (frontier.empty() ? 0 :
                         dist[frontier[0]]);
        int next_level = cur_level + 1;

        std::vector<int> local_update(total_vertices, -1);
        for (int v : new_verts)
            if (dist[v] == -1)
                local_update[v] = next_level;

        // Allreduce with MAX to share discoveries
        std::vector<int> global_update(total_vertices, -1);
        MPI_Allreduce(local_update.data(), global_update.data(),
                      total_vertices, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

        // Apply updates
        frontier.clear();
        bool any_new = false;
        for (int v = 0; v < total_vertices; v++) {
            if (dist[v] == -1 && global_update[v] != -1) {
                dist[v] = global_update[v];
                any_new = true;
                if (v % size == rank) frontier.push_back(v);
            }
        }

        // Check globally if any new vertices were discovered
        int local_any = any_new ? 1 : 0;
        int global_any = 0;
        MPI_Allreduce(&local_any, &global_any, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        global_active = (global_any > 0);
    }

    if (rank == 0) return dist;
    return {};
}
