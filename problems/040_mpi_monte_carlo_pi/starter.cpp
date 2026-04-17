#include <mpi.h>
#include <cstdlib>

// Monte Carlo pi: each rank generates n_local samples, reduce total hits.
// Returns pi estimate on rank 0, 0.0 on others.
double mpi_monte_carlo_pi(long long n_local, unsigned base_seed) {
    // TODO: implement
    return 0.0;
}
