#include <mpi.h>
#include <vector>

std::vector<double> reduce_scatter_sum(const std::vector<double>& data, int n) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<int> recvcounts(size, n);
    std::vector<double> result(n);

    MPI_Reduce_scatter(data.data(), result.data(), recvcounts.data(),
                       MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    return result;
}
