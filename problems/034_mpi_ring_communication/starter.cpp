#include <mpi.h>

// Token ring: rank 0 starts token=0, each rank adds its rank, passes to next.
// Returns final token (sum 0..size-1) on rank 0, 0 on others.
int mpi_ring_sum() {
    // TODO: implement ring communication
    return 0;
}
