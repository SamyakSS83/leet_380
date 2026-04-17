#include <mpi.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include <climits>

struct Stats { double min, max, sum; long long count; };

Stats gather_stats(const std::vector<double>& local_data) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double local_min = local_data.empty() ?  1e300 : *std::min_element(local_data.begin(), local_data.end());
    double local_max = local_data.empty() ? -1e300 : *std::max_element(local_data.begin(), local_data.end());
    double local_sum = std::accumulate(local_data.begin(), local_data.end(), 0.0);
    long long local_count = (long long)local_data.size();

    double global_min, global_max, global_sum;
    long long global_count;

    MPI_Reduce(&local_min,   &global_min,   1, MPI_DOUBLE,    MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_max,   &global_max,   1, MPI_DOUBLE,    MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_sum,   &global_sum,   1, MPI_DOUBLE,    MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_count, &global_count, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        if (global_count == 0) return Stats{};
        return Stats{global_min, global_max, global_sum, global_count};
    }
    return Stats{};
}
