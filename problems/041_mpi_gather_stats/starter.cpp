#include <mpi.h>
#include <vector>
#include <climits>

struct Stats { double min, max, sum; long long count; };

// Each rank has local_data. Compute global min, max, sum, count.
// Returns populated Stats on rank 0, zeroed Stats on others.
Stats gather_stats(const std::vector<double>& local_data) {
    // TODO: implement
    return Stats{};
}
