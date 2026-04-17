#include <mpi.h>
#include <vector>

// local_A: local_rows rows of A (n/size), length = local_rows * k
// B: full k×m matrix replicated on all ranks, length = k*m
// Returns full C (n×m) on rank 0, empty on others.
std::vector<double> distributed_matmul(
    const std::vector<double>& local_A, int local_rows,
    const std::vector<double>& B, int n, int k, int m) {
    // TODO: implement
    return {};
}
