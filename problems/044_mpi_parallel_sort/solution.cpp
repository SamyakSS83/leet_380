#include <mpi.h>
#include <vector>
#include <algorithm>

std::vector<int> parallel_sample_sort(std::vector<int> local_data) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Step 1: local sort
    std::sort(local_data.begin(), local_data.end());

    // Step 2: select size samples from local data
    int local_n = (int)local_data.size();
    std::vector<int> local_samples(size);
    for (int i = 0; i < size; i++) {
        int idx = local_n > 0 ? (long long)i * local_n / size : 0;
        local_samples[i] = local_n > 0 ? local_data[idx] : 0;
    }

    // Step 3: gather all samples to rank 0
    std::vector<int> all_samples(size * size);
    MPI_Gather(local_samples.data(), size, MPI_INT,
               all_samples.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

    // Rank 0 picks size-1 splitters
    std::vector<int> splitters(size - 1);
    if (rank == 0) {
        std::sort(all_samples.begin(), all_samples.end());
        for (int i = 0; i < size - 1; i++)
            splitters[i] = all_samples[(i + 1) * size];
    }

    // Step 4: broadcast splitters
    MPI_Bcast(splitters.data(), size - 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 5: partition local data into size buckets
    std::vector<int> send_counts(size, 0), send_displs(size, 0);
    // Count elements per bucket
    for (int x : local_data) {
        int bucket = (int)(std::lower_bound(splitters.begin(), splitters.end(), x) - splitters.begin());
        // upper_bound to handle equal elements going to the right bucket
        bucket = (int)(std::upper_bound(splitters.begin(), splitters.end(), x) - splitters.begin());
        // Clamp
        if (bucket >= size) bucket = size - 1;
        send_counts[bucket]++;
    }
    send_displs[0] = 0;
    for (int i = 1; i < size; i++) send_displs[i] = send_displs[i-1] + send_counts[i-1];

    // Build send buffer partitioned by bucket
    std::vector<int> send_buf(local_n);
    std::vector<int> bucket_pos = send_displs;
    for (int x : local_data) {
        int bucket = (int)(std::upper_bound(splitters.begin(), splitters.end(), x) - splitters.begin());
        if (bucket >= size) bucket = size - 1;
        send_buf[bucket_pos[bucket]++] = x;
    }

    // Step 6: exchange bucket sizes
    std::vector<int> recv_counts(size), recv_displs(size);
    MPI_Alltoall(send_counts.data(), 1, MPI_INT, recv_counts.data(), 1, MPI_INT, MPI_COMM_WORLD);
    recv_displs[0] = 0;
    for (int i = 1; i < size; i++) recv_displs[i] = recv_displs[i-1] + recv_counts[i-1];
    int recv_total = recv_displs[size-1] + recv_counts[size-1];

    std::vector<int> recv_buf(recv_total);
    MPI_Alltoallv(send_buf.data(), send_counts.data(), send_displs.data(), MPI_INT,
                  recv_buf.data(), recv_counts.data(), recv_displs.data(), MPI_INT,
                  MPI_COMM_WORLD);

    // Step 7: sort received data
    std::sort(recv_buf.begin(), recv_buf.end());
    return recv_buf;
}
