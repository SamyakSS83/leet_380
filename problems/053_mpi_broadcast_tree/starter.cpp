#include <mpi.h>
#include <vector>

// Broadcast data from root to all ranks using only MPI_Send/MPI_Recv.
// Use binomial tree algorithm.
// After call, all ranks have root's original data.
void tree_broadcast(std::vector<int>& data, int root) {
    // TODO: implement
}
