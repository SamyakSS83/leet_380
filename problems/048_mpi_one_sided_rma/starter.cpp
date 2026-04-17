#include <mpi.h>

// Use MPI one-sided RMA (no MPI_Reduce/MPI_Allreduce) to find the global max.
// Returns global max on rank 0, local_value on other ranks.
double rma_global_max(double local_value) {
    // TODO: implement
    return local_value;
}
