#include <mpi.h>
#include <vector>

// data: length n*size on every rank.
// Returns: length-n segment of the global element-wise sum belonging to this rank.
std::vector<double> reduce_scatter_sum(const std::vector<double>& data, int n) {
    // TODO: implement
    return {};
}
