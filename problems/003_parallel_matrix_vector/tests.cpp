// tests.cpp for problem 003 — Parallel Matrix-Vector Multiply
#include "harness.h"

std::vector<double> parallel_matvec(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& x);

void test_empty() {
    auto r = parallel_matvec({}, {});
    ASSERT_TRUE(r.empty(), "expected empty result");
}

void test_1x1() {
    auto r = parallel_matvec({{5.0}}, {3.0});
    ASSERT_APPROX(r[0], 15.0);
}

void test_2x2() {
    auto r = parallel_matvec({{1.0,2.0},{3.0,4.0}}, {1.0,1.0});
    ASSERT_APPROX(r[0], 3.0);
    ASSERT_APPROX(r[1], 7.0);
}

void test_identity() {
    std::vector<std::vector<double>> I = {{1,0,0},{0,1,0},{0,0,1}};
    std::vector<double> x = {5.0, 6.0, 7.0};
    auto r = parallel_matvec(I, x);
    ASSERT_VEC_APPROX(r, x);
}

void test_zero_matrix() {
    std::vector<std::vector<double>> Z(4, std::vector<double>(4, 0.0));
    std::vector<double> x = {1,2,3,4};
    auto r = parallel_matvec(Z, x);
    std::vector<double> expected(4, 0.0);
    ASSERT_VEC_APPROX(r, expected);
}

void test_rectangular() {
    // 2x3 * 3 = 2
    auto r = parallel_matvec({{1,2,3},{4,5,6}}, {1,1,1});
    ASSERT_APPROX(r[0], 6.0);
    ASSERT_APPROX(r[1], 15.0);
}

void test_large() {
    int m = 200, n = 200;
    auto A = rand_mat(m, n, 42);
    auto x = rand_vec(n, 7, -1.0, 1.0);
    std::vector<double> expected(m, 0.0);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            expected[i] += A[i][j] * x[j];
    auto r = parallel_matvec(A, x);
    ASSERT_VEC_APPROX(r, expected, 1e-6, 1e-9);
}

void test_single_row() {
    auto r = parallel_matvec({{2.0, 3.0, 4.0}}, {1.0, 2.0, 3.0});
    ASSERT_APPROX(r[0], 20.0);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("1x1", test_1x1);
    RUN_TEST("2x2", test_2x2);
    RUN_TEST("identity matrix", test_identity);
    RUN_TEST("zero matrix", test_zero_matrix);
    RUN_TEST("rectangular 2x3", test_rectangular);
    RUN_TEST("large 200x200", test_large);
    RUN_TEST("single row", test_single_row);
    print_results();
    return 0;
}
