#include <mpi.h>
#include <vector>

// Scatter full_array (rank 0 only), each rank sums its chunk,
// verify global sum equals reference sum from rank 0.
// Returns true on all ranks if consistent.
bool verify_distributed_sum(const std::vector<double>& full_array) {
    // TODO: implement
    return false;
}
