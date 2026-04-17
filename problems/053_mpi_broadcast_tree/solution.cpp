#include <mpi.h>
#include <vector>

void tree_broadcast(std::vector<int>& data, int root) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int vrank = (rank - root + size) % size;

    // Phase 1: broadcast the vector size
    int n = (int)data.size();
    {
        // Receive from parent
        int mask = 1;
        while (mask < size) {
            if (vrank & mask) {
                int parent = ((vrank ^ mask) + root) % size;
                MPI_Recv(&n, 1, MPI_INT, parent, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                data.resize(n);
                break;
            }
            mask <<= 1;
        }
        // Send to children (process bits from high to low)
        mask >>= 1;
        while (mask >= 1) {
            if (!(vrank & mask) && (vrank + mask) < size) {
                int child = ((vrank + mask) + root) % size;
                MPI_Send(&n, 1, MPI_INT, child, 10, MPI_COMM_WORLD);
            }
            mask >>= 1;
        }
    }

    // Phase 2: broadcast the data
    {
        int mask = 1;
        while (mask < size) {
            if (vrank & mask) {
                int parent = ((vrank ^ mask) + root) % size;
                MPI_Recv(data.data(), n, MPI_INT, parent, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                break;
            }
            mask <<= 1;
        }
        mask >>= 1;
        while (mask >= 1) {
            if (!(vrank & mask) && (vrank + mask) < size) {
                int child = ((vrank + mask) + root) % size;
                MPI_Send(data.data(), n, MPI_INT, child, 11, MPI_COMM_WORLD);
            }
            mask >>= 1;
        }
    }
}
