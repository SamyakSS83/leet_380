#include <mpi.h>
#include <omp.h>
#include <vector>
#include <climits>

struct MinLoc {
    double value;
    int rank;
    int local_index;
};

// Hybrid global minimum with location.
// OMP local reduction per rank, then MPI custom op for global min.
// Returns MinLoc on rank 0, default MinLoc{0.0,0,0} on others.
MinLoc hybrid_global_min(const std::vector<double>& local_data) {
    // TODO: implement
    return {0.0, 0, 0};
}
