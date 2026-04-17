#include <vector>
#include <omp.h>

// Multiply N×N matrices A and B (row-major flat vectors).
// Return C = A * B as a flat row-major vector.
std::vector<double> parallel_matmul(const std::vector<double>& A,
                                     const std::vector<double>& B, int N) {
    // TODO: implement
    return std::vector<double>(N * N, 0.0);
}
