#include <mpi.h>
#include <vector>

static long long fib(int n) {
    if (n <= 1) return n;
    long long a = 0, b = 1;
    for (int i = 2; i <= n; i++) { long long c = a + b; a = b; b = c; }
    return b;
}

std::vector<long long> dynamic_load_balance(int num_tasks) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int TAG_TASK   = 1;
    const int TAG_RESULT = 2;
    const int POISON     = -1;

    if (rank == 0) {
        // Master
        std::vector<long long> results(num_tasks, 0);
        int next_task = 0;
        int active_workers = size - 1;

        // Seed each worker with a task
        for (int w = 1; w < size && next_task < num_tasks; w++, next_task++) {
            MPI_Send(&next_task, 1, MPI_INT, w, TAG_TASK, MPI_COMM_WORLD);
        }
        // If fewer tasks than workers, send poison to idle workers immediately
        for (int w = next_task + 1; w < size && next_task < size - 1; w++) {
            int poison = POISON;
            MPI_Send(&poison, 1, MPI_INT, w, TAG_TASK, MPI_COMM_WORLD);
            active_workers--;
        }

        // Receive results and dispatch more tasks
        while (active_workers > 0) {
            long long buf[2]; // buf[0]=task_index, buf[1]=result
            MPI_Status status;
            MPI_Recv(buf, 2, MPI_LONG_LONG, MPI_ANY_SOURCE, TAG_RESULT, MPI_COMM_WORLD, &status);
            int worker = status.MPI_SOURCE;
            results[(int)buf[0]] = buf[1];

            if (next_task < num_tasks) {
                MPI_Send(&next_task, 1, MPI_INT, worker, TAG_TASK, MPI_COMM_WORLD);
                next_task++;
            } else {
                int poison = POISON;
                MPI_Send(&poison, 1, MPI_INT, worker, TAG_TASK, MPI_COMM_WORLD);
                active_workers--;
            }
        }
        return results;
    } else {
        // Worker
        while (true) {
            int task_idx;
            MPI_Recv(&task_idx, 1, MPI_INT, 0, TAG_TASK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (task_idx == POISON) break;
            long long result = fib(task_idx % 35 + 1);
            long long buf[2] = { (long long)task_idx, result };
            MPI_Send(buf, 2, MPI_LONG_LONG, 0, TAG_RESULT, MPI_COMM_WORLD);
        }
        return {};
    }
}
