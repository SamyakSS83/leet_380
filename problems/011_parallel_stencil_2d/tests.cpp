// tests.cpp for problem 011 — Parallel 2D Stencil
#include "harness.h"

std::vector<double> stencil_2d(std::vector<double> u, int N, int iters);

void test_zero_iters() {
    std::vector<double> u = {1,2,3,4,5,6,7,8,9};
    auto r = stencil_2d(u, 3, 0);
    ASSERT_VEC_APPROX(r, u);
}

void test_uniform_stays_uniform() {
    std::vector<double> u(25, 3.0);
    auto r = stencil_2d(u, 5, 10);
    for (double x : r) ASSERT_APPROX(x, 3.0);
}

void test_boundaries_fixed() {
    int N = 5;
    std::vector<double> u(N*N, 0.0);
    // Set some boundary values
    for (int j = 0; j < N; j++) u[0*N+j] = 1.0;
    auto r = stencil_2d(u, N, 50);
    for (int j = 0; j < N; j++) ASSERT_APPROX(r[0*N+j], 1.0);
}

void test_one_iteration_center() {
    int N = 3;
    std::vector<double> u = {0,0,0, 0,5,0, 0,0,0};
    auto r = stencil_2d(u, N, 1);
    // center: (0+0+0+0+5)/5=1
    ASSERT_APPROX(r[4], 1.0);
}

void test_size_correct() {
    int N = 10;
    std::vector<double> u(N*N, 1.0);
    auto r = stencil_2d(u, N, 5);
    ASSERT_EQ((int)r.size(), N*N);
}

void test_large() {
    int N = 100;
    std::vector<double> u(N*N, 0.0);
    for (int j = 0; j < N; j++) u[0*N+j] = 100.0;
    auto r = stencil_2d(u, N, 20);
    ASSERT_EQ((int)r.size(), N*N);
    // Top boundary stays fixed
    for (int j = 0; j < N; j++) ASSERT_APPROX(r[0*N+j], 100.0);
}

int main() {
    RUN_TEST("zero iterations", test_zero_iters);
    RUN_TEST("uniform stays uniform", test_uniform_stays_uniform);
    RUN_TEST("boundaries fixed", test_boundaries_fixed);
    RUN_TEST("one iter center", test_one_iteration_center);
    RUN_TEST("output size correct", test_size_correct);
    RUN_TEST("large 100x100", test_large);
    print_results();
    return 0;
}
