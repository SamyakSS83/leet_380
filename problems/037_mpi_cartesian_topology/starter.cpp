#include <mpi.h>
#include <vector>

// Perform 2D halo exchange on (local_N+2)x(local_N+2) grid (includes ghost cells).
// cart_comm is a 2D Cartesian communicator.
void halo_exchange_2d(std::vector<double>& grid, int local_N, MPI_Comm cart_comm) {
    // TODO: exchange top/bottom rows and left/right columns with neighbors
}

// Setup cart communicator and call halo_exchange_2d.
// Returns the grid after halo exchange on all ranks.
std::vector<double> mpi_cart_halo(int local_N) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // Assume size is a perfect square
    int dims[2] = {0, 0};
    MPI_Dims_create(size, 2, dims);
    int periods[2] = {1, 1};
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

    int rank;
    MPI_Comm_rank(cart_comm, &rank);
    int W = local_N + 2;
    std::vector<double> grid(W * W, (double)rank);
    // Interior cells: set to rank, halos start as 0
    halo_exchange_2d(grid, local_N, cart_comm);
    MPI_Comm_free(&cart_comm);
    return grid;
}
