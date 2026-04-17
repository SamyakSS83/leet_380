#include <mpi.h>
#include <omp.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdio>

std::vector<std::string> hybrid_hello(int num_threads) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    omp_set_num_threads(num_threads);

    // Each rank builds its local messages
    std::vector<std::string> local_msgs(num_threads);
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        char buf[256];
        snprintf(buf, sizeof(buf),
                 "Hello from rank %d thread %d of %d ranks %d threads",
                 rank, tid, size, num_threads);
        local_msgs[tid] = std::string(buf);
    }

    // Serialize: each message padded to fixed width
    const int MSG_LEN = 128;
    std::vector<char> local_buf(num_threads * MSG_LEN, '\0');
    for (int t = 0; t < num_threads; t++) {
        const auto& s = local_msgs[t];
        int copy_len = std::min((int)s.size(), MSG_LEN - 1);
        std::copy(s.begin(), s.begin() + copy_len, local_buf.begin() + t * MSG_LEN);
    }

    // All ranks have the same num_threads so each sends num_threads*MSG_LEN chars
    int sendcount = num_threads * MSG_LEN;
    std::vector<char> recv_buf;
    if (rank == 0) recv_buf.resize(size * sendcount, '\0');

    MPI_Gather(local_buf.data(), sendcount, MPI_CHAR,
               rank == 0 ? recv_buf.data() : nullptr, sendcount, MPI_CHAR,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::vector<std::string> all_msgs;
        all_msgs.reserve(size * num_threads);
        for (int r = 0; r < size; r++) {
            for (int t = 0; t < num_threads; t++) {
                const char* p = recv_buf.data() + (r * sendcount) + (t * MSG_LEN);
                all_msgs.emplace_back(p);
            }
        }
        std::sort(all_msgs.begin(), all_msgs.end());
        return all_msgs;
    }
    return {};
}
