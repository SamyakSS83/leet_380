#include <vector>
#include <omp.h>

// Compute histogram of v with B bins, where element v[i] goes into bin v[i] % B.
// Use atomic updates or per-thread local histograms.
std::vector<int> parallel_histogram(const std::vector<int>& v, int B) {
    // TODO: implement
    return std::vector<int>(B, 0);
}
