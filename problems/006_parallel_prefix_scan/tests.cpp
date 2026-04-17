// tests.cpp for problem 006 — Parallel Prefix Scan
#include "harness.h"
#include <numeric>

std::vector<double> parallel_prefix_scan(const std::vector<double>& v);

void test_empty() {
    auto r = parallel_prefix_scan({});
    ASSERT_TRUE(r.empty(), "expected empty");
}

void test_single() {
    auto r = parallel_prefix_scan({7.0});
    ASSERT_EQ((int)r.size(), 1);
    ASSERT_APPROX(r[0], 7.0);
}

void test_basic() {
    auto r = parallel_prefix_scan({1.0, 2.0, 3.0, 4.0, 5.0});
    std::vector<double> exp = {1, 3, 6, 10, 15};
    ASSERT_VEC_APPROX(r, exp);
}

void test_all_ones() {
    std::vector<double> v(10, 1.0);
    auto r = parallel_prefix_scan(v);
    for (int i = 0; i < 10; i++) ASSERT_APPROX(r[i], (double)(i+1));
}

void test_negatives() {
    auto r = parallel_prefix_scan({-1.0, -2.0, -3.0});
    std::vector<double> exp = {-1, -3, -6};
    ASSERT_VEC_APPROX(r, exp);
}

void test_large() {
    auto v = rand_vec(10000, 42, 0.0, 1.0);
    auto r = parallel_prefix_scan(v);
    ASSERT_EQ((int)r.size(), 10000);
    // verify sequential result
    std::vector<double> expected(10000);
    std::partial_sum(v.begin(), v.end(), expected.begin());
    ASSERT_VEC_APPROX(r, expected, 1e-6, 1e-6);
}

void test_two_elements() {
    auto r = parallel_prefix_scan({3.0, 5.0});
    ASSERT_APPROX(r[0], 3.0);
    ASSERT_APPROX(r[1], 8.0);
}

void test_large_values() {
    auto r = parallel_prefix_scan({1e6, 2e6, 3e6});
    ASSERT_APPROX(r[0], 1e6);
    ASSERT_APPROX(r[1], 3e6);
    ASSERT_APPROX(r[2], 6e6);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("single", test_single);
    RUN_TEST("basic 1..5", test_basic);
    RUN_TEST("all ones", test_all_ones);
    RUN_TEST("negatives", test_negatives);
    RUN_TEST("large 10k", test_large);
    RUN_TEST("two elements", test_two_elements);
    RUN_TEST("large values", test_large_values);
    print_results();
    return 0;
}
