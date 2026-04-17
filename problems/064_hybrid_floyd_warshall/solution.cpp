#include <mpi.h>
#include <omp.h>
#include <vector>
#include <climits>
#include <algorithm>

std::vector<std::vector<int>> hybrid_floyd_warshall(
    std::vector<std::vector<int>> local_dist, int n) {

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_rows = (int)local_dist.size(); // n / size
    const int INF = INT_MAX / 2;

    // Pivot row buffer
    std::vector<int> pivot_row(n);

    for (int k = 0; k < n; k++) {
        // Rank that owns row k
        int owner = k / local_rows;  // rows are distributed: rank r owns rows [r*local_rows .. (r+1)*local_rows-1]
        int local_k = k - owner * local_rows;

        if (rank == owner) {
            pivot_row = local_dist[local_k];
        }

        MPI_Bcast(pivot_row.data(), n, MPI_INT, owner, MPI_COMM_WORLD);

        // Update local rows using OMP
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < local_rows; i++) {
            int dik = local_dist[i][k];
            if (dik == INF) continue;
            for (int j = 0; j < n; j++) {
                int dkj = pivot_row[j];
                if (dkj == INF) continue;
                int newval = dik + dkj;
                if (newval < local_dist[i][j])
                    local_dist[i][j] = newval;
            }
        }
    }

    // Gather all rows to rank 0
    if (rank == 0) {
        std::vector<std::vector<int>> full_dist(n, std::vector<int>(n));
        // Copy local rows
        for (int i = 0; i < local_rows; i++)
            full_dist[i] = local_dist[i];
        // Receive from other ranks
        for (int r = 1; r < size; r++) {
            int row_start = r * local_rows;
            for (int i = 0; i < local_rows; i++) {
                MPI_Recv(full_dist[row_start + i].data(), n, MPI_INT,
                         r, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        return full_dist;
    } else {
        for (int i = 0; i < local_rows; i++) {
            MPI_Send(local_dist[i].data(), n, MPI_INT, 0, i, MPI_COMM_WORLD);
        }
        return {};
    }
}
