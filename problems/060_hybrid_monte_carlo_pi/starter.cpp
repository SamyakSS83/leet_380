#include <mpi.h>
#include <omp.h>
#include <cstdlib>
#include <cmath>

// Hybrid Monte Carlo pi estimation.
// Each OMP thread generates samples_per_thread samples.
// MPI_Reduce aggregates counts.
// Returns pi estimate on rank 0, 0.0 on others.
double hybrid_monte_carlo_pi(long long samples_per_thread) {
    // TODO: implement
    return 0.0;
}
