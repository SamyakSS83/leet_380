#include <mpi.h>
#include <vector>

// Gather variable-length vectors: rank r provides r+1 doubles.
// Returns concatenated data on rank 0, empty on others.
std::vector<double> gather_variable(const std::vector<double>& local_data) {
    // TODO: implement
    return {};
}
