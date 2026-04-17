#include <mpi.h>
#include <omp.h>
#include <vector>

// MPI scatter + OMP local reduction + MPI reduce to get global sum.
// data provided on rank 0, empty on others.
// Returns global sum on rank 0, 0.0 on others.
double hybrid_sum(const std::vector<double>& data) {
    // TODO: implement
    return 0.0;
}
