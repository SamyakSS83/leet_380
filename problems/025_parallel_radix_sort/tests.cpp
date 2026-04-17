// tests.cpp for problem 025 — Parallel Radix Sort
#include "harness.h"
#include <algorithm>

std::vector<unsigned> parallel_radix_sort(std::vector<unsigned> v);

void test_empty() {
    auto r = parallel_radix_sort({});
    ASSERT_TRUE(r.empty(), "empty");
}

void test_single() {
    auto r = parallel_radix_sort({42u});
    ASSERT_EQ((int)r.size(), 1); ASSERT_EQ(r[0], 42u);
}

void test_basic() {
    auto r = parallel_radix_sort({5,3,1,4,2});
    std::vector<unsigned> exp = {1,2,3,4,5};
    ASSERT_VEC_EQ(r, exp);
}

void test_all_same() {
    std::vector<unsigned> v(100, 7u);
    auto r = parallel_radix_sort(v);
    for (auto x : r) ASSERT_EQ(x, 7u);
}

void test_large_values() {
    std::vector<unsigned> v = {0xFFFFFFFF, 0, 0x80000000, 1};
    auto r = parallel_radix_sort(v);
    ASSERT_EQ(r[0], 0u); ASSERT_EQ(r[3], 0xFFFFFFFFu);
}

void test_random() {
    srand(42);
    std::vector<unsigned> v(10000);
    for (auto& x : v) x = (unsigned)rand();
    auto exp = v;
    std::sort(exp.begin(), exp.end());
    auto r = parallel_radix_sort(v);
    ASSERT_VEC_EQ(r, exp);
}

void test_large() {
    srand(7);
    std::vector<unsigned> v(100000);
    for (auto& x : v) x = (unsigned)rand();
    auto exp = v;
    std::sort(exp.begin(), exp.end());
    auto r = parallel_radix_sort(v);
    ASSERT_VEC_EQ(r, exp);
}

void test_two_elements() {
    auto r = parallel_radix_sort({10u, 5u});
    ASSERT_EQ(r[0], 5u); ASSERT_EQ(r[1], 10u);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("single", test_single);
    RUN_TEST("basic 1-5", test_basic);
    RUN_TEST("all same", test_all_same);
    RUN_TEST("large 32-bit values", test_large_values);
    RUN_TEST("random 10k", test_random);
    RUN_TEST("large 100k", test_large);
    RUN_TEST("two elements", test_two_elements);
    print_results();
    return 0;
}
