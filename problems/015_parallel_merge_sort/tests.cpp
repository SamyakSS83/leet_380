// tests.cpp for problem 015 — Parallel Merge Sort
#include "harness.h"
#include <algorithm>

void parallel_merge_sort(std::vector<int>& v, int cutoff);

void test_empty() {
    std::vector<int> v;
    parallel_merge_sort(v, 100);
    ASSERT_TRUE(v.empty(), "empty stays empty");
}

void test_single() {
    std::vector<int> v = {42};
    parallel_merge_sort(v, 100);
    ASSERT_EQ(v[0], 42);
}

void test_sorted() {
    std::vector<int> v = {1,2,3,4,5};
    parallel_merge_sort(v, 2);
    for (int i = 0; i < 4; i++) ASSERT_TRUE(v[i] <= v[i+1], "not sorted");
}

void test_reverse_sorted() {
    std::vector<int> v = {5,4,3,2,1};
    parallel_merge_sort(v, 2);
    std::vector<int> exp = {1,2,3,4,5};
    ASSERT_VEC_EQ(v, exp);
}

void test_random() {
    auto v = rand_vec_i(10000, 42, 0, 9999);
    auto expected = v;
    std::sort(expected.begin(), expected.end());
    parallel_merge_sort(v, 256);
    ASSERT_VEC_EQ(v, expected);
}

void test_duplicates() {
    std::vector<int> v = {3,1,4,1,5,9,2,6,5,3,5};
    auto exp = v;
    std::sort(exp.begin(), exp.end());
    parallel_merge_sort(v, 4);
    ASSERT_VEC_EQ(v, exp);
}

void test_large() {
    auto v = rand_vec_i(100000, 7, -1000, 1000);
    auto exp = v;
    std::sort(exp.begin(), exp.end());
    parallel_merge_sort(v, 1000);
    ASSERT_VEC_EQ(v, exp);
}

void test_cutoff_equals_size() {
    auto v = rand_vec_i(100, 1, 0, 100);
    auto exp = v;
    std::sort(exp.begin(), exp.end());
    parallel_merge_sort(v, 100);
    ASSERT_VEC_EQ(v, exp);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("single", test_single);
    RUN_TEST("already sorted", test_sorted);
    RUN_TEST("reverse sorted", test_reverse_sorted);
    RUN_TEST("random 10k", test_random);
    RUN_TEST("duplicates", test_duplicates);
    RUN_TEST("large 100k", test_large);
    RUN_TEST("cutoff equals size", test_cutoff_equals_size);
    print_results();
    return 0;
}
