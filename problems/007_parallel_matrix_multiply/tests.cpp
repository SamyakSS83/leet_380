// tests.cpp for problem 007 — Parallel Matrix Multiply
#include "harness.h"

std::vector<double> parallel_matmul(const std::vector<double>& A,
                                     const std::vector<double>& B, int N);

void test_empty() {
    auto r = parallel_matmul({}, {}, 0);
    ASSERT_TRUE(r.empty(), "expected empty");
}

void test_1x1() {
    auto r = parallel_matmul({3.0}, {4.0}, 1);
    ASSERT_APPROX(r[0], 12.0);
}

void test_2x2() {
    std::vector<double> A = {1,2,3,4}, B = {5,6,7,8};
    auto r = parallel_matmul(A, B, 2);
    ASSERT_APPROX(r[0], 19.0); ASSERT_APPROX(r[1], 22.0);
    ASSERT_APPROX(r[2], 43.0); ASSERT_APPROX(r[3], 50.0);
}

void test_identity() {
    // C = I * A = A
    int N = 3;
    std::vector<double> I = {1,0,0, 0,1,0, 0,0,1};
    std::vector<double> A = {1,2,3, 4,5,6, 7,8,9};
    auto r = parallel_matmul(I, A, N);
    ASSERT_VEC_APPROX(r, A);
}

void test_zero_matrix() {
    int N = 3;
    std::vector<double> Z(9, 0.0), A = {1,2,3,4,5,6,7,8,9};
    auto r = parallel_matmul(Z, A, N);
    std::vector<double> expected(9, 0.0);
    ASSERT_VEC_APPROX(r, expected);
}

void test_large() {
    int N = 64;
    srand(42);
    std::vector<double> A(N*N), B(N*N);
    for (auto& x : A) x = (double)rand()/RAND_MAX - 0.5;
    for (auto& x : B) x = (double)rand()/RAND_MAX - 0.5;
    auto r = parallel_matmul(A, B, N);
    // verify one element
    double expected = 0.0;
    for (int k = 0; k < N; k++) expected += A[0*N+k] * B[k*N+0];
    ASSERT_APPROX(r[0], expected, 1e-9, 1e-9);
}

void test_2x2_with_negatives() {
    std::vector<double> A = {1,-1,2,-2}, B = {3,4,-5,6};
    auto r = parallel_matmul(A, B, 2);
    // [1*3+(-1)*(-5), 1*4+(-1)*6, 2*3+(-2)*(-5), 2*4+(-2)*6]
    // [8, -2, 16, -4]
    ASSERT_APPROX(r[0], 8.0); ASSERT_APPROX(r[1], -2.0);
    ASSERT_APPROX(r[2], 16.0); ASSERT_APPROX(r[3], -4.0);
}

int main() {
    RUN_TEST("empty N=0", test_empty);
    RUN_TEST("1x1", test_1x1);
    RUN_TEST("2x2 basic", test_2x2);
    RUN_TEST("identity * A = A", test_identity);
    RUN_TEST("zero matrix", test_zero_matrix);
    RUN_TEST("large 64x64", test_large);
    RUN_TEST("2x2 negatives", test_2x2_with_negatives);
    print_results();
    return 0;
}
