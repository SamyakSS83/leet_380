// tests.cpp for problem 024 — Parallel Sparse MatVec
#include "harness.h"

std::vector<double> parallel_spmv(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<double>& values,
    const std::vector<double>& x, int nrows);

void test_identity() {
    // 3x3 identity
    std::vector<int> rp = {0,1,2,3};
    std::vector<int> ci = {0,1,2};
    std::vector<double> v = {1,1,1};
    std::vector<double> x = {4,5,6};
    auto y = parallel_spmv(rp, ci, v, x, 3);
    ASSERT_VEC_APPROX(y, x);
}

void test_diagonal() {
    std::vector<int> rp = {0,1,2,3};
    std::vector<int> ci = {0,1,2};
    std::vector<double> v = {2,3,4};
    std::vector<double> x = {1,2,3};
    auto y = parallel_spmv(rp, ci, v, x, 3);
    std::vector<double> exp = {2,6,12};
    ASSERT_VEC_APPROX(y, exp);
}

void test_empty_rows() {
    // Zero matrix
    std::vector<int> rp = {0,0,0,0};
    std::vector<int> ci = {};
    std::vector<double> v = {};
    std::vector<double> x = {1,2,3};
    auto y = parallel_spmv(rp, ci, v, x, 3);
    std::vector<double> exp = {0,0,0};
    ASSERT_VEC_APPROX(y, exp);
}

void test_dense_row() {
    // One row touching all columns
    std::vector<int> rp = {0,3};
    std::vector<int> ci = {0,1,2};
    std::vector<double> v = {1,2,3};
    std::vector<double> x = {1,1,1};
    auto y = parallel_spmv(rp, ci, v, x, 1);
    ASSERT_APPROX(y[0], 6.0);
}

void test_large() {
    // Build diagonal matrix 1000x1000
    int n = 1000;
    std::vector<int> rp(n+1);
    std::vector<int> ci(n);
    std::vector<double> v(n);
    auto x = rand_vec(n, 42, 0.0, 1.0);
    for (int i = 0; i < n; i++) { rp[i] = i; ci[i] = i; v[i] = (double)(i+1); }
    rp[n] = n;
    auto y = parallel_spmv(rp, ci, v, x, n);
    for (int i = 0; i < n; i++) ASSERT_APPROX(y[i], (double)(i+1)*x[i]);
}

int main() {
    RUN_TEST("identity matrix", test_identity);
    RUN_TEST("diagonal matrix", test_diagonal);
    RUN_TEST("empty rows (zero matrix)", test_empty_rows);
    RUN_TEST("dense single row", test_dense_row);
    RUN_TEST("large diagonal 1000x1000", test_large);
    print_results();
    return 0;
}
