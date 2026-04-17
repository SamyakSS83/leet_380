#include <mpi.h>
#include <vector>

// Write local_data to filename at byte offset rank * local_data.size() * sizeof(double).
// All ranks participate collectively.
void parallel_file_write(const std::vector<double>& local_data, const char* filename) {
    // TODO: implement
}
