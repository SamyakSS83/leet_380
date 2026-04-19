#include <vector>
#include <omp.h>

std::vector<double> parallel_prefix_scan(const std::vector<double>& v) {
    int n = (int)v.size();
    if (n == 0) return {};

    // Hillis-Steele inclusive scan with iterative doubling.
    std::vector<double> curr = v;
    std::vector<double> next(n);

    for (int offset = 1; offset < n; offset <<= 1) {
        #pragma omp parallel for
        for (int i = 0; i < n; i++) {
            if (i >= offset) next[i] = curr[i] + curr[i - offset];
            else next[i] = curr[i];
        }
        curr.swap(next);
    }

    return curr;
}
