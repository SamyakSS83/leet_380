#include <mpi.h>
#include <omp.h>
#include <vector>

// Compute C = A * B with MPI row distribution and OMP parallelism.
// local_A: local_rows x k matrix (row-major)
// B: k x m matrix (row-major), replicated on all ranks
// n: total rows of A across all ranks
// Returns full C (n x m) on rank 0, empty on others.
std::vector<double> hybrid_matmul(
    const std::vector<double>& local_A, int local_rows,
    const std::vector<double>& B, int n, int k, int m) {
    // TODO: implement
    return {};
}
