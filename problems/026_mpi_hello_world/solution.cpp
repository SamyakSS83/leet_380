#include <mpi.h>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>

std::vector<std::string> mpi_hello(int argc, char** argv) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int MSG_LEN = 64;
    char local_msg[MSG_LEN];
    snprintf(local_msg, MSG_LEN, "Hello from rank %d of %d", rank, size);

    std::vector<char> all_msgs;
    if (rank == 0) all_msgs.resize(size * MSG_LEN);
    MPI_Gather(local_msg, MSG_LEN, MPI_CHAR,
               rank==0 ? all_msgs.data() : nullptr, MSG_LEN, MPI_CHAR,
               0, MPI_COMM_WORLD);

    std::vector<std::string> result;
    if (rank == 0) {
        for (int r = 0; r < size; r++)
            result.push_back(std::string(&all_msgs[r * MSG_LEN]));
        std::sort(result.begin(), result.end());
    }
    return result;
}
