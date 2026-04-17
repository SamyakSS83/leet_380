// tests.cpp for problem 020 — Max Subarray Sum
#include "harness.h"

double max_subarray_sum(const std::vector<double>& v);

void test_empty() { ASSERT_APPROX(max_subarray_sum({}), 0.0); }

void test_single_positive() { ASSERT_APPROX(max_subarray_sum({5.0}), 5.0); }

void test_single_negative() { ASSERT_APPROX(max_subarray_sum({-3.0}), 0.0); }

void test_classic() {
    ASSERT_APPROX(max_subarray_sum({-2,1,-3,4,-1,2,1,-5,4}), 6.0);
}

void test_all_positive() {
    ASSERT_APPROX(max_subarray_sum({1,2,3,4,5}), 15.0);
}

void test_all_negative() {
    ASSERT_APPROX(max_subarray_sum({-1,-2,-3}), 0.0);
}

void test_mixed() {
    ASSERT_APPROX(max_subarray_sum({-1, 5, -2, 3, -1}), 6.0);  // [5,-2,3]
}

void test_large() {
    std::vector<double> v(10000, -1.0);
    v[5000] = 10000.0;
    ASSERT_APPROX(max_subarray_sum(v), 10000.0);
}

void test_two_spikes() {
    // Two positive spikes, pick the larger
    ASSERT_APPROX(max_subarray_sum({-5, 3, -10, 7, -5}), 7.0);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("single positive", test_single_positive);
    RUN_TEST("single negative", test_single_negative);
    RUN_TEST("classic example", test_classic);
    RUN_TEST("all positive", test_all_positive);
    RUN_TEST("all negative", test_all_negative);
    RUN_TEST("mixed", test_mixed);
    RUN_TEST("large with spike", test_large);
    RUN_TEST("two spikes", test_two_spikes);
    print_results();
    return 0;
}
