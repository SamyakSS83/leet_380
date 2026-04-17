#include <vector>
#include <omp.h>

std::vector<double> parallel_spmv(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<double>& values,
    const std::vector<double>& x, int nrows) {
    std::vector<double> y(nrows, 0.0);
    #pragma omp parallel for schedule(dynamic, 64)
    for (int i = 0; i < nrows; i++) {
        double sum = 0.0;
        for (int k = row_ptr[i]; k < row_ptr[i+1]; k++)
            sum += values[k] * x[col_idx[k]];
        y[i] = sum;
    }
    return y;
}
