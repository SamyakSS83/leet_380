#include <mpi.h>

struct ArgMax { double value; int rank; };

// Find the global argmax using a custom MPI_Op.
// Returns {max_value, rank_of_max} on rank 0.
// Tie-break: lower rank wins.
ArgMax mpi_argmax(double local_value) {
    // TODO: implement
    return {0.0, 0};
}
