#include <vector>
#include <omp.h>

// Sum using naive per-thread array slot (false sharing likely).
double sum_with_false_sharing(const std::vector<double>& v, int nthreads) {
    // TODO: implement — use array of doubles for per-thread partials
    return 0.0;
}

// Sum using padded per-thread slot (no false sharing).
double sum_with_padding(const std::vector<double>& v, int nthreads) {
    // TODO: implement — use alignas(64) struct for per-thread partials
    return 0.0;
}
