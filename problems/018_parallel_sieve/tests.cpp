// tests.cpp for problem 018 — Parallel Sieve
#include "harness.h"

std::vector<int> parallel_sieve(int N);

void test_n0() { ASSERT_TRUE(parallel_sieve(0).empty(), "N=0 empty"); }
void test_n1() { ASSERT_TRUE(parallel_sieve(1).empty(), "N=1 empty"); }
void test_n2() { auto r = parallel_sieve(2); ASSERT_EQ((int)r.size(), 1); ASSERT_EQ(r[0], 2); }
void test_n10() {
    auto r = parallel_sieve(10);
    std::vector<int> exp = {2,3,5,7};
    ASSERT_VEC_EQ(r, exp);
}
void test_n30() {
    auto r = parallel_sieve(30);
    std::vector<int> exp = {2,3,5,7,11,13,17,19,23,29};
    ASSERT_VEC_EQ(r, exp);
}
void test_count_1000() {
    // 168 primes below 1000
    auto r = parallel_sieve(999);
    ASSERT_EQ((int)r.size(), 168);
}
void test_largest_prime_below_100() {
    auto r = parallel_sieve(100);
    ASSERT_EQ(r.back(), 97);
}
void test_large() {
    auto r = parallel_sieve(1000000);
    // 78498 primes up to 10^6
    ASSERT_EQ((int)r.size(), 78498);
}
void test_sorted() {
    auto r = parallel_sieve(10000);
    for (int i = 1; i < (int)r.size(); i++)
        ASSERT_TRUE(r[i] > r[i-1], "not sorted");
}

int main() {
    RUN_TEST("N=0", test_n0);
    RUN_TEST("N=1", test_n1);
    RUN_TEST("N=2", test_n2);
    RUN_TEST("N=10", test_n10);
    RUN_TEST("N=30", test_n30);
    RUN_TEST("count below 1000", test_count_1000);
    RUN_TEST("largest prime below 100", test_largest_prime_below_100);
    RUN_TEST("large N=1M count", test_large);
    RUN_TEST("sorted output", test_sorted);
    print_results();
    return 0;
}
