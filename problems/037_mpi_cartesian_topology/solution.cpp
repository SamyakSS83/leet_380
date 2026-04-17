#include <mpi.h>
#include <vector>

void halo_exchange_2d(std::vector<double>& grid, int local_N, MPI_Comm cart_comm) {
    int W = local_N + 2;
    int up, down, left, right;
    MPI_Cart_shift(cart_comm, 0, 1, &up, &down);
    MPI_Cart_shift(cart_comm, 1, 1, &left, &right);

    // Exchange rows (up/down)
    MPI_Sendrecv(&grid[1*W+1], local_N, MPI_DOUBLE, up, 0,
                 &grid[(local_N+1)*W+1], local_N, MPI_DOUBLE, down, 0,
                 cart_comm, MPI_STATUS_IGNORE);
    MPI_Sendrecv(&grid[local_N*W+1], local_N, MPI_DOUBLE, down, 1,
                 &grid[0*W+1], local_N, MPI_DOUBLE, up, 1,
                 cart_comm, MPI_STATUS_IGNORE);

    // Exchange columns using derived type or manual loop
    std::vector<double> send_col(local_N), recv_col(local_N);
    // Left
    for (int i = 0; i < local_N; i++) send_col[i] = grid[(i+1)*W+1];
    MPI_Sendrecv(send_col.data(), local_N, MPI_DOUBLE, left, 2,
                 recv_col.data(), local_N, MPI_DOUBLE, right, 2,
                 cart_comm, MPI_STATUS_IGNORE);
    for (int i = 0; i < local_N; i++) grid[(i+1)*W+(local_N+1)] = recv_col[i];
    // Right
    for (int i = 0; i < local_N; i++) send_col[i] = grid[(i+1)*W+local_N];
    MPI_Sendrecv(send_col.data(), local_N, MPI_DOUBLE, right, 3,
                 recv_col.data(), local_N, MPI_DOUBLE, left, 3,
                 cart_comm, MPI_STATUS_IGNORE);
    for (int i = 0; i < local_N; i++) grid[(i+1)*W+0] = recv_col[i];
}

std::vector<double> mpi_cart_halo(int local_N) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int dims[2] = {0,0};
    MPI_Dims_create(size, 2, dims);
    int periods[2] = {1,1};
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cart_comm);

    int rank;
    MPI_Comm_rank(cart_comm, &rank);
    int W = local_N + 2;
    std::vector<double> grid(W*W, 0.0);
    for (int i = 1; i <= local_N; i++)
        for (int j = 1; j <= local_N; j++)
            grid[i*W+j] = (double)rank;
    halo_exchange_2d(grid, local_N, cart_comm);
    MPI_Comm_free(&cart_comm);
    return grid;
}
