#include <mpi.h>
#include <vector>

void my_bcast(std::vector<double>& buf, int root) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int n = (int)buf.size();
    int rel = (rank - root + size) % size;

    for (int mask = 1; mask < size; mask <<= 1) {
        if (rel < mask) {
            // I have data, send to rel+mask if it exists
            int dest_rel = rel + mask;
            if (dest_rel < size) {
                int dest = (dest_rel + root) % size;
                MPI_Send(buf.data(), n, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
            }
        } else if (rel < (mask << 1)) {
            // I receive from rel-mask
            int src_rel = rel - mask;
            int src = (src_rel + root) % size;
            MPI_Recv(buf.data(), n, MPI_DOUBLE, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
}
