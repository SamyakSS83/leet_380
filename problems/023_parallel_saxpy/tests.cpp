// tests.cpp for problem 023 — Parallel SAXPY
#include "harness.h"

void parallel_saxpy(double a, const std::vector<double>& x, std::vector<double>& y);

void test_empty() {
    std::vector<double> x, y;
    parallel_saxpy(2.0, x, y);
    ASSERT_TRUE(y.empty(), "empty ok");
}

void test_basic() {
    std::vector<double> x = {1,2,3}, y = {4,5,6};
    parallel_saxpy(2.0, x, y);
    std::vector<double> exp = {6,9,12};
    ASSERT_VEC_APPROX(y, exp);
}

void test_a_zero() {
    std::vector<double> x = {1,2,3}, y = {4,5,6};
    parallel_saxpy(0.0, x, y);
    std::vector<double> exp = {4,5,6};
    ASSERT_VEC_APPROX(y, exp);
}

void test_a_one() {
    std::vector<double> x = {1,2,3}, y = {1,1,1};
    parallel_saxpy(1.0, x, y);
    std::vector<double> exp = {2,3,4};
    ASSERT_VEC_APPROX(y, exp);
}

void test_negative_a() {
    std::vector<double> x = {1,2,3}, y = {0,0,0};
    parallel_saxpy(-1.0, x, y);
    std::vector<double> exp = {-1,-2,-3};
    ASSERT_VEC_APPROX(y, exp);
}

void test_large() {
    auto x = rand_vec(100000, 42, -1.0, 1.0);
    auto y = rand_vec(100000, 7, -1.0, 1.0);
    auto y_exp = y;
    for (int i = 0; i < 100000; i++) y_exp[i] = 3.14 * x[i] + y_exp[i];
    parallel_saxpy(3.14, x, y);
    ASSERT_VEC_APPROX(y, y_exp, 1e-9, 1e-12);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("basic a=2", test_basic);
    RUN_TEST("a=0", test_a_zero);
    RUN_TEST("a=1", test_a_one);
    RUN_TEST("negative a", test_negative_a);
    RUN_TEST("large 100k", test_large);
    print_results();
    return 0;
}
