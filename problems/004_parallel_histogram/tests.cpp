// tests.cpp for problem 004 — Parallel Histogram
#include "harness.h"

std::vector<int> parallel_histogram(const std::vector<int>& v, int B);

void test_empty() {
    auto r = parallel_histogram({}, 5);
    ASSERT_EQ((int)r.size(), 5);
    for (int x : r) ASSERT_EQ(x, 0);
}

void test_basic() {
    auto r = parallel_histogram({0,1,2,3,4,5}, 3);
    ASSERT_EQ(r[0], 2); ASSERT_EQ(r[1], 2); ASSERT_EQ(r[2], 2);
}

void test_one_bin() {
    auto r = parallel_histogram({5, 10, 15}, 5);
    ASSERT_EQ(r[0], 3);
    for (int i = 1; i < 5; i++) ASSERT_EQ(r[i], 0);
}

void test_single_bin_count() {
    auto r = parallel_histogram({7, 7, 7, 7}, 3);
    ASSERT_EQ(r[1], 4);  // 7%3 = 1
}

void test_uniform() {
    std::vector<int> v(1000);
    for (int i = 0; i < 1000; i++) v[i] = i;
    auto r = parallel_histogram(v, 10);
    for (int b = 0; b < 10; b++) ASSERT_EQ(r[b], 100);
}

void test_large() {
    auto v = rand_vec_i(100000, 42, 0, 999);
    auto r = parallel_histogram(v, 10);
    int total = 0;
    for (int x : r) total += x;
    ASSERT_EQ(total, 100000);
}

void test_sum_equals_n() {
    auto v = rand_vec_i(50000, 1, 0, 9999);
    int B = 7;
    auto r = parallel_histogram(v, B);
    int total = 0;
    for (int x : r) total += x;
    ASSERT_EQ(total, 50000);
}

int main() {
    RUN_TEST("empty array", test_empty);
    RUN_TEST("basic mod 3", test_basic);
    RUN_TEST("one bin hits", test_one_bin);
    RUN_TEST("single bin count", test_single_bin_count);
    RUN_TEST("uniform distribution", test_uniform);
    RUN_TEST("large array sum", test_large);
    RUN_TEST("sum equals n", test_sum_equals_n);
    print_results();
    return 0;
}
