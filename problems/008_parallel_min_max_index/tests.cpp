// tests.cpp for problem 008 — Parallel Min and Max with Index
#include "harness.h"
#include <limits>
#include <algorithm>

// MinMaxResult is defined in solve.cpp (included before this file by the build wrapper)
MinMaxResult parallel_min_max_index(const std::vector<double>& v);

void test_single() {
    auto r = parallel_min_max_index({5.0});
    ASSERT_APPROX(r.min_val, 5.0); ASSERT_EQ(r.min_idx, 0);
    ASSERT_APPROX(r.max_val, 5.0); ASSERT_EQ(r.max_idx, 0);
}

void test_basic() {
    auto r = parallel_min_max_index({3.0, 1.0, 4.0, 1.5, 9.0, 2.0});
    ASSERT_APPROX(r.min_val, 1.0); ASSERT_EQ(r.min_idx, 1);
    ASSERT_APPROX(r.max_val, 9.0); ASSERT_EQ(r.max_idx, 4);
}

void test_sorted() {
    auto r = parallel_min_max_index({1.0, 2.0, 3.0, 4.0, 5.0});
    ASSERT_EQ(r.min_idx, 0); ASSERT_EQ(r.max_idx, 4);
}

void test_reverse_sorted() {
    auto r = parallel_min_max_index({5.0, 4.0, 3.0, 2.0, 1.0});
    ASSERT_EQ(r.min_idx, 4); ASSERT_EQ(r.max_idx, 0);
}

void test_all_same() {
    auto r = parallel_min_max_index({7.0, 7.0, 7.0});
    ASSERT_APPROX(r.min_val, 7.0); ASSERT_EQ(r.min_idx, 0);
    ASSERT_APPROX(r.max_val, 7.0); ASSERT_EQ(r.max_idx, 0);
}

void test_negatives() {
    auto r = parallel_min_max_index({-5.0, -1.0, -3.0, -10.0, -2.0});
    ASSERT_APPROX(r.min_val, -10.0); ASSERT_EQ(r.min_idx, 3);
    ASSERT_APPROX(r.max_val, -1.0); ASSERT_EQ(r.max_idx, 1);
}

void test_large() {
    auto v = rand_vec(100000, 42, -100.0, 100.0);
    auto r = parallel_min_max_index(v);
    auto it_min = std::min_element(v.begin(), v.end());
    auto it_max = std::max_element(v.begin(), v.end());
    ASSERT_APPROX(r.min_val, *it_min);
    ASSERT_APPROX(r.max_val, *it_max);
    ASSERT_EQ(r.min_idx, (int)(it_min - v.begin()));
    ASSERT_EQ(r.max_idx, (int)(it_max - v.begin()));
}

void test_two_elements() {
    auto r = parallel_min_max_index({10.0, 5.0});
    ASSERT_EQ(r.min_idx, 1); ASSERT_EQ(r.max_idx, 0);
}

int main() {
    RUN_TEST("single element", test_single);
    RUN_TEST("basic array", test_basic);
    RUN_TEST("sorted ascending", test_sorted);
    RUN_TEST("sorted descending", test_reverse_sorted);
    RUN_TEST("all same", test_all_same);
    RUN_TEST("all negative", test_negatives);
    RUN_TEST("large 100k", test_large);
    RUN_TEST("two elements", test_two_elements);
    print_results();
    return 0;
}
