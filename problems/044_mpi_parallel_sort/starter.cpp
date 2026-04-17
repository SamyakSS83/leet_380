#include <mpi.h>
#include <vector>
#include <algorithm>

// Parallel sample sort. local_data is this rank's portion of unsorted integers.
// Returns sorted local portion; rank 0 has smallest, rank P-1 has largest.
std::vector<int> parallel_sample_sort(std::vector<int> local_data) {
    // TODO: implement
    return local_data;
}
