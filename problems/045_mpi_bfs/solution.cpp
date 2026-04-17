#include <mpi.h>
#include <vector>
#include <queue>
#include <algorithm>

std::vector<int> distributed_bfs(
    const std::vector<std::vector<int>>& local_adj,
    int total_vertices,
    int source) {

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_verts = (int)local_adj.size();
    std::vector<int> dist(total_vertices, -1);

    // Initialize source
    if (source % size == rank) {
        dist[source] = 0;
    }

    // Current frontier for this rank
    std::vector<int> frontier;
    if (source % size == rank) frontier.push_back(source);

    int level = 0;
    while (true) {
        // Collect vertices to send to each rank
        std::vector<std::vector<int>> to_send(size);
        for (int v : frontier) {
            int local_idx = v / size;
            for (int nb : local_adj[local_idx]) {
                int nb_owner = nb % size;
                to_send[nb_owner].push_back(nb);
            }
        }

        // Build Alltoallv buffers
        std::vector<int> send_counts(size), send_displs(size);
        for (int r = 0; r < size; r++) send_counts[r] = (int)to_send[r].size();
        send_displs[0] = 0;
        for (int r = 1; r < size; r++) send_displs[r] = send_displs[r-1] + send_counts[r-1];
        int total_send = send_displs[size-1] + send_counts[size-1];
        std::vector<int> send_buf(total_send);
        for (int r = 0; r < size; r++)
            std::copy(to_send[r].begin(), to_send[r].end(), send_buf.begin() + send_displs[r]);

        std::vector<int> recv_counts(size), recv_displs(size);
        MPI_Alltoall(send_counts.data(), 1, MPI_INT, recv_counts.data(), 1, MPI_INT, MPI_COMM_WORLD);
        recv_displs[0] = 0;
        for (int r = 1; r < size; r++) recv_displs[r] = recv_displs[r-1] + recv_counts[r-1];
        int total_recv = recv_displs[size-1] + recv_counts[size-1];
        std::vector<int> recv_buf(total_recv);
        MPI_Alltoallv(send_buf.data(), send_counts.data(), send_displs.data(), MPI_INT,
                      recv_buf.data(), recv_counts.data(), recv_displs.data(), MPI_INT,
                      MPI_COMM_WORLD);

        // Process received candidates
        std::vector<int> new_frontier;
        for (int v : recv_buf) {
            if (dist[v] == -1) {
                dist[v] = level + 1;
                new_frontier.push_back(v);
            }
        }
        frontier = new_frontier;
        level++;

        // Check if any rank has a non-empty frontier
        int local_has = (int)frontier.empty() ? 0 : 1;
        int global_has = 0;
        MPI_Allreduce(&local_has, &global_has, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        if (global_has == 0) break;
    }

    // Gather distances to rank 0
    // Each rank knows dist for its own vertices; combine with MPI_Reduce
    // For vertices not owned by this rank, dist is -1 unless it's already set globally.
    // Actually dist is maintained globally on all ranks — just reduce with MAX to rank 0.
    if (rank == 0) {
        std::vector<int> global_dist(total_vertices);
        MPI_Reduce(dist.data(), global_dist.data(), total_vertices, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
        return global_dist;
    } else {
        MPI_Reduce(dist.data(), nullptr, total_vertices, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
        return {};
    }
}
