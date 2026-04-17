#include <vector>
#include <omp.h>

double nested_parallel_sum(const std::vector<std::vector<double>>& mat) {
    int rows = (int)mat.size();
    if (rows == 0) return 0.0;
    omp_set_nested(1);
    double total = 0.0;
    #pragma omp parallel for reduction(+:total) schedule(static) num_threads(2)
    for (int i = 0; i < rows; i++) {
        double row_sum = 0.0;
        int cols = (int)mat[i].size();
        #pragma omp parallel for reduction(+:row_sum) schedule(static) num_threads(2)
        for (int j = 0; j < cols; j++) row_sum += mat[i][j];
        total += row_sum;
    }
    return total;
}
