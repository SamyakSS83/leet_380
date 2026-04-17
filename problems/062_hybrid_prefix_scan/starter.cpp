#include <mpi.h>
#include <omp.h>
#include <vector>

// Hybrid exclusive prefix sum.
// Phase 1: OMP local exclusive scan.
// Phase 2: MPI_Exscan for inter-rank offset.
// Phase 3: OMP add offset to local results.
// Returns local portion of global exclusive prefix sum.
std::vector<double> hybrid_prefix_scan(const std::vector<double>& local_data) {
    // TODO: implement
    return {};
}
