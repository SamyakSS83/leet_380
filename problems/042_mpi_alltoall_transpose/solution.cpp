#include <mpi.h>
#include <vector>

// Distributed transpose using MPI_Alltoall.
// Each rank holds block = n/size rows. After transpose each rank holds block columns.
// Output layout: result[row * block + c] = A[row][rank*block + c]
std::vector<double> distributed_transpose(const std::vector<double>& local_rows, int n) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int block = n / size;  // rows per rank, also columns per rank after transpose

    // Pack send buffer: for each destination rank d, send block*block elements
    // These are the block columns [d*block..(d+1)*block) of my local rows
    // send_buf[d][local_row * block + c] = local_rows[local_row * n + d*block + c]
    std::vector<double> send_buf(block * n);  // block rows, n cols = size * block cols
    for (int d = 0; d < size; d++) {
        for (int r = 0; r < block; r++) {
            for (int c = 0; c < block; c++) {
                send_buf[d * block * block + r * block + c] = local_rows[r * n + d * block + c];
            }
        }
    }

    std::vector<double> recv_buf(block * n);
    MPI_Alltoall(send_buf.data(), block * block, MPI_DOUBLE,
                 recv_buf.data(), block * block, MPI_DOUBLE,
                 MPI_COMM_WORLD);

    // After alltoall: recv_buf[s * block*block + r * block + c]
    // came from rank s's row r, columns [rank*block + c]
    // Final layout: result[row * block + c] = A[row][rank*block + c]
    // row = s * block + r, so result[(s*block + r)*block + c] = recv_buf[s*block*block + r*block + c]
    // That's already the right order! Just return recv_buf transposed within each source block.
    // Actually we need result[global_row * block + c] where global_row = s*block + r
    // recv_buf[s*block^2 + r*block + c] -> result[(s*block+r)*block + c]
    // This is just a reshape — same memory layout already correct.
    return recv_buf;
}
