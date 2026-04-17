// tests.cpp for problem 001 — Parallel Sum
#include "harness.h"

double parallel_sum(const std::vector<double>& v);

void test_empty() {
    double r = parallel_sum({});
    ASSERT_APPROX(r, 0.0);
}

void test_single() {
    double r = parallel_sum({42.0});
    ASSERT_APPROX(r, 42.0);
}

void test_basic() {
    double r = parallel_sum({1.0, 2.0, 3.0, 4.0, 5.0});
    ASSERT_APPROX(r, 15.0);
}

void test_negatives() {
    double r = parallel_sum({-1.0, -2.0, -3.0});
    ASSERT_APPROX(r, -6.0);
}

void test_mixed() {
    double r = parallel_sum({1.0, -1.0, 2.0, -2.0, 3.0, -3.0});
    ASSERT_APPROX(r, 0.0);
}

void test_large() {
    auto v = rand_vec(100000, 42, 0.0, 1.0);
    double expected = 0.0;
    for (double x : v) expected += x;
    double r = parallel_sum(v);
    ASSERT_APPROX(r, expected, 1e-6, 1e-4);
}

void test_all_same() {
    std::vector<double> v(1000, 3.14);
    double r = parallel_sum(v);
    ASSERT_APPROX(r, 3.14 * 1000, 1e-6, 1e-4);
}

void test_two_elements() {
    double r = parallel_sum({100.0, 200.0});
    ASSERT_APPROX(r, 300.0);
}

void test_large_values() {
    double r = parallel_sum({1e6, 2e6, 3e6});
    ASSERT_APPROX(r, 6e6);
}

void test_very_large() {
    auto v = rand_vec(1000000, 7, -1.0, 1.0);
    double expected = 0.0;
    for (double x : v) expected += x;
    double r = parallel_sum(v);
    ASSERT_APPROX(r, expected, 1e-4, 1e-2);
}

int main() {
    RUN_TEST("empty array", test_empty);
    RUN_TEST("single element", test_single);
    RUN_TEST("basic 1..5", test_basic);
    RUN_TEST("all negatives", test_negatives);
    RUN_TEST("mixed signs", test_mixed);
    RUN_TEST("large array 100k", test_large);
    RUN_TEST("all same value", test_all_same);
    RUN_TEST("two elements", test_two_elements);
    RUN_TEST("large values", test_large_values);
    RUN_TEST("1M elements", test_very_large);
    print_results();
    return 0;
}
