#include <vector>
#include <omp.h>

// CSR sparse matrix-vector product: y = A * x
// row_ptr[i..i+1) indexes into col_idx and values for row i.
std::vector<double> parallel_spmv(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<double>& values,
    const std::vector<double>& x, int nrows) {
    // TODO: implement
    return std::vector<double>(nrows, 0.0);
}
