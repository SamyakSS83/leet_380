// tests.cpp for problem 012 — OMP Schedule Clauses
#include "harness.h"
#include <cmath>

double irregular_sum(const std::vector<int>& work_sizes, double base);

void test_empty() {
    ASSERT_APPROX(irregular_sum({}, 2.0), 0.0);
}

void test_all_zeros() {
    // 2^0 = 1, sum of 5 = 5
    ASSERT_APPROX(irregular_sum({0,0,0,0,0}, 2.0), 5.0);
}

void test_basic() {
    // 2^1 + 2^2 + 2^3 = 2+4+8 = 14
    ASSERT_APPROX(irregular_sum({1,2,3}, 2.0), 14.0);
}

void test_base_one() {
    // 1^anything = 1, sum of 4 = 4
    ASSERT_APPROX(irregular_sum({0,5,10,15}, 1.0), 4.0);
}

void test_irregular() {
    std::vector<int> ws = {0,1,2,3,4,5,6,7,8,9};
    double expected = 0.0;
    for (int w : ws) expected += std::pow(2.0, w);
    ASSERT_APPROX(irregular_sum(ws, 2.0), expected, 1e-6, 1e-6);
}

void test_large() {
    auto wi = rand_vec_i(10000, 42, 0, 20);
    double expected = 0.0;
    for (int w : wi) expected += std::pow(1.5, w);
    ASSERT_APPROX(irregular_sum(wi, 1.5), expected, 1e-4, 1e-2);
}

void test_single() {
    ASSERT_APPROX(irregular_sum({10}, 2.0), 1024.0);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("all zeros", test_all_zeros);
    RUN_TEST("basic powers", test_basic);
    RUN_TEST("base one", test_base_one);
    RUN_TEST("irregular work", test_irregular);
    RUN_TEST("large 10k", test_large);
    RUN_TEST("single element", test_single);
    print_results();
    return 0;
}
