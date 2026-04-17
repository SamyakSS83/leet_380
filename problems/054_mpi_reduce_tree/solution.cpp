#include <mpi.h>

double tree_reduce_sum(double local_value, int root) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int vrank = (rank - root + size) % size;
    double partial = local_value;

    int mask = 1;
    while (mask < size) {
        if (vrank & mask) {
            // Send partial sum to parent and stop
            int parent_vrank = vrank ^ mask;
            int parent = (parent_vrank + root) % size;
            MPI_Send(&partial, 1, MPI_DOUBLE, parent, 20, MPI_COMM_WORLD);
            return partial; // not the root — return value undefined
        } else {
            // Receive from child if it exists
            int child_vrank = vrank | mask;
            if (child_vrank < size) {
                int child = (child_vrank + root) % size;
                double recv_val;
                MPI_Recv(&recv_val, 1, MPI_DOUBLE, child, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                partial += recv_val;
            }
        }
        mask <<= 1;
    }

    // Only root (vrank == 0) reaches here
    return partial;
}
