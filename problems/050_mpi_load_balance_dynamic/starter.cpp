#include <mpi.h>
#include <vector>

// Dynamic load balancing: rank 0 distributes task indices to workers on demand.
// Workers compute fib(task_index % 35 + 1) and report back.
// Returns results vector on rank 0, empty on other ranks.
std::vector<long long> dynamic_load_balance(int num_tasks) {
    // TODO: implement
    return {};
}
