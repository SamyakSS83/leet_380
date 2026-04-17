#include <mpi.h>
#include <vector>

void parallel_file_write(const std::vector<double>& local_data, const char* filename) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, filename,
                  MPI_MODE_CREATE | MPI_MODE_WRONLY,
                  MPI_INFO_NULL, &fh);

    MPI_Offset offset = (MPI_Offset)rank * (MPI_Offset)local_data.size() * sizeof(double);
    MPI_Status status;
    MPI_File_write_at(fh, offset, local_data.data(), (int)local_data.size(),
                      MPI_DOUBLE, &status);

    MPI_File_close(&fh);
}
