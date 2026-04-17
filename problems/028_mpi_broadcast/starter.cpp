#include <mpi.h>
#include <vector>

// Broadcast buf from root to all ranks using only MPI_Send/MPI_Recv (binomial tree).
void my_bcast(std::vector<double>& buf, int root) {
    // TODO: implement binomial tree broadcast
}
