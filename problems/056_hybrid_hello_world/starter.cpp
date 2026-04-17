#include <mpi.h>
#include <omp.h>
#include <vector>
#include <string>

// Each thread on each rank emits "Hello from rank R thread T of S ranks N threads".
// Collect all messages, return sorted on rank 0. Others return empty.
std::vector<std::string> hybrid_hello(int num_threads) {
    // TODO: implement
    return {};
}
