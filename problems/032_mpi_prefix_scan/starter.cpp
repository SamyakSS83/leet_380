#include <mpi.h>

// Compute exclusive prefix sum using MPI_Exscan.
// Rank r returns sum of local_val from ranks 0..r-1; rank 0 returns 0.
double mpi_exclusive_prefix(double local_val) {
    // TODO: implement using MPI_Exscan
    return 0.0;
}
