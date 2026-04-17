#include <vector>
#include <algorithm>
#include <omp.h>

// Sort v in-place using parallel merge sort with OMP tasks.
// Use std::sort for chunks smaller than cutoff.
void parallel_merge_sort(std::vector<int>& v, int cutoff) {
    // TODO: implement
}
