// tests.cpp for problem 010 — Parallel 1D Stencil
#include "harness.h"

std::vector<double> stencil_1d(std::vector<double> u, int iters);

void test_zero_iters() {
    std::vector<double> u = {1.0, 2.0, 3.0, 4.0, 5.0};
    auto r = stencil_1d(u, 0);
    ASSERT_VEC_APPROX(r, u);
}

void test_boundaries_fixed() {
    std::vector<double> u = {10.0, 0.0, 0.0, 0.0, 20.0};
    auto r = stencil_1d(u, 100);
    ASSERT_APPROX(r[0], 10.0);
    ASSERT_APPROX(r[4], 20.0);
}

void test_uniform_stays_uniform() {
    std::vector<double> u(10, 5.0);
    auto r = stencil_1d(u, 10);
    for (double x : r) ASSERT_APPROX(x, 5.0);
}

void test_one_iteration() {
    std::vector<double> u = {0.0, 9.0, 0.0};
    auto r = stencil_1d(u, 1);
    ASSERT_APPROX(r[0], 0.0);
    ASSERT_APPROX(r[1], 3.0);  // (0+9+0)/3
    ASSERT_APPROX(r[2], 0.0);
}

void test_convergence() {
    // With many iterations, interior should converge to linear interpolation
    int n = 100;
    std::vector<double> u(n, 0.0);
    u[0] = 0.0; u[n-1] = 100.0;
    auto r = stencil_1d(u, 100000);
    // Check midpoint near 50 (Jacobi averaging kernel converges slowly)
    ASSERT_TRUE(fabs(r[n/2] - 50.0) < 2.0, "midpoint should be near 50");
}

void test_large() {
    int n = 10000;
    std::vector<double> u(n, 0.0);
    u[0] = 1.0; u[n-1] = 1.0;
    auto r = stencil_1d(u, 10);
    ASSERT_EQ((int)r.size(), n);
    ASSERT_APPROX(r[0], 1.0);
    ASSERT_APPROX(r[n-1], 1.0);
}

void test_small_array() {
    std::vector<double> u = {0.0, 6.0, 0.0};
    auto r = stencil_1d(u, 2);
    ASSERT_APPROX(r[0], 0.0); ASSERT_APPROX(r[2], 0.0);
    // After iter1: [0, 2, 0]; iter2: [0, 2/3, 0]
    ASSERT_APPROX(r[1], 2.0/3.0, 1e-6, 1e-9);
}

int main() {
    RUN_TEST("zero iterations", test_zero_iters);
    RUN_TEST("boundaries fixed", test_boundaries_fixed);
    RUN_TEST("uniform stays uniform", test_uniform_stays_uniform);
    RUN_TEST("one iteration", test_one_iteration);
    RUN_TEST("convergence to linear", test_convergence);
    RUN_TEST("large n=10000", test_large);
    RUN_TEST("small 3-element 2 iters", test_small_array);
    print_results();
    return 0;
}
