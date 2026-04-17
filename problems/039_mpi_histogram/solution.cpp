#include <mpi.h>
#include <vector>

std::vector<int> mpi_histogram(const std::vector<int>& local_data, int B) {
    std::vector<int> local_hist(B, 0);
    for (int v : local_data) local_hist[v % B]++;
    std::vector<int> global_hist(B, 0);
    MPI_Allreduce(local_hist.data(), global_hist.data(), B, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    return global_hist;
}
