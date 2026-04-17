// tests.cpp for problem 016 — Parallel Quicksort
#include "harness.h"
#include <algorithm>

void parallel_quicksort(std::vector<int>& v, int cutoff);

void test_empty() {
    std::vector<int> v;
    parallel_quicksort(v, 64);
    ASSERT_TRUE(v.empty(), "empty stays empty");
}

void test_single() {
    std::vector<int> v = {7};
    parallel_quicksort(v, 64);
    ASSERT_EQ(v[0], 7);
}

void test_two() {
    std::vector<int> v = {2, 1};
    parallel_quicksort(v, 1);
    ASSERT_EQ(v[0], 1); ASSERT_EQ(v[1], 2);
}

void test_sorted() {
    std::vector<int> v = {1,2,3,4,5,6,7,8};
    parallel_quicksort(v, 4);
    for (int i = 0; i < 7; i++) ASSERT_TRUE(v[i] <= v[i+1], "not sorted");
}

void test_reverse() {
    std::vector<int> v = {8,7,6,5,4,3,2,1};
    auto exp = v; std::sort(exp.begin(), exp.end());
    parallel_quicksort(v, 4);
    ASSERT_VEC_EQ(v, exp);
}

void test_random() {
    auto v = rand_vec_i(10000, 42, 0, 9999);
    auto exp = v; std::sort(exp.begin(), exp.end());
    parallel_quicksort(v, 256);
    ASSERT_VEC_EQ(v, exp);
}

void test_all_same() {
    std::vector<int> v(1000, 5);
    parallel_quicksort(v, 100);
    for (int x : v) ASSERT_EQ(x, 5);
}

void test_large() {
    auto v = rand_vec_i(100000, 99, -500, 500);
    auto exp = v; std::sort(exp.begin(), exp.end());
    parallel_quicksort(v, 1000);
    ASSERT_VEC_EQ(v, exp);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("single", test_single);
    RUN_TEST("two elements", test_two);
    RUN_TEST("already sorted", test_sorted);
    RUN_TEST("reverse sorted", test_reverse);
    RUN_TEST("random 10k", test_random);
    RUN_TEST("all same", test_all_same);
    RUN_TEST("large 100k", test_large);
    print_results();
    return 0;
}
