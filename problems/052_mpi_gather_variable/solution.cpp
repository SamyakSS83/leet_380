#include <mpi.h>
#include <vector>

std::vector<double> gather_variable(const std::vector<double>& local_data) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int sendcount = (int)local_data.size();

    std::vector<int> recvcounts(size);
    std::vector<int> displs(size);
    std::vector<double> gathered;

    if (rank == 0) {
        int offset = 0;
        for (int r = 0; r < size; r++) {
            recvcounts[r] = r + 1;
            displs[r] = offset;
            offset += r + 1;
        }
        gathered.resize(offset);
    }

    MPI_Gatherv(
        local_data.data(), sendcount, MPI_DOUBLE,
        rank == 0 ? gathered.data() : nullptr,
        rank == 0 ? recvcounts.data() : nullptr,
        rank == 0 ? displs.data() : nullptr,
        MPI_DOUBLE, 0, MPI_COMM_WORLD
    );

    return (rank == 0) ? gathered : std::vector<double>{};
}
