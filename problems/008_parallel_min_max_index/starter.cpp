#include <vector>
#include <omp.h>
#include <limits>

struct MinMaxResult {
    double min_val;
    int    min_idx;
    double max_val;
    int    max_idx;
};

// Find min value/index and max value/index in parallel.
MinMaxResult parallel_min_max_index(const std::vector<double>& v) {
    // TODO: implement
    return {0.0, -1, 0.0, -1};
}
