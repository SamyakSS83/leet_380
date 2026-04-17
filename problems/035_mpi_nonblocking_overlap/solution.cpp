#include <mpi.h>
#include <vector>
#include <utility>

std::pair<double, std::vector<std::vector<double>>>
    mpi_nonblocking_collect(const std::vector<double>& local_data) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int n = (int)local_data.size();

    std::vector<std::vector<double>> all_data;
    std::vector<MPI_Request> reqs;

    if (rank == 0) {
        all_data.resize(size, std::vector<double>(n));
        // Copy own data directly; post Irecv only from non-zero ranks
        all_data[0] = local_data;
        reqs.resize(size - 1);
        for (int r = 1; r < size; r++)
            MPI_Irecv(all_data[r].data(), n, MPI_DOUBLE, r, 0, MPI_COMM_WORLD, &reqs[r-1]);
    } else {
        reqs.resize(1);
        MPI_Isend(const_cast<double*>(local_data.data()), n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &reqs[0]);
    }

    // Overlap: compute local sum of squares
    double sos = 0.0;
    for (double x : local_data) sos += x * x;

    // Wait for communications
    MPI_Waitall((int)reqs.size(), reqs.data(), MPI_STATUSES_IGNORE);

    return {sos, all_data};
}
