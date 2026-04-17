// tests.cpp for problem 013 — OMP Tasks Fibonacci
#include "harness.h"

long long parallel_fibonacci(int n, int cutoff);

static long long fib_expected[] = {0,1,1,2,3,5,8,13,21,34,55,89,144,233,377,
    610,987,1597,2584,4181,6765,10946,17711,28657,46368,75025,121393,196418,
    317811,514229,832040,1346269,2178309,3524578,5702887,9227465,14930352,
    24157817,39088169,63245986,102334155,165580141,267914296,433494437,701408733};

void test_base_cases() {
    ASSERT_EQ(parallel_fibonacci(0, 5), 0LL);
    ASSERT_EQ(parallel_fibonacci(1, 5), 1LL);
}

void test_small() {
    for (int n = 0; n <= 10; n++)
        ASSERT_EQ(parallel_fibonacci(n, 5), fib_expected[n]);
}

void test_cutoff_above_n() {
    ASSERT_EQ(parallel_fibonacci(8, 10), 21LL);
}

void test_medium() {
    ASSERT_EQ(parallel_fibonacci(20, 10), 6765LL);
}

void test_large() {
    ASSERT_EQ(parallel_fibonacci(40, 10), 102334155LL);
}

void test_various_cutoffs() {
    for (int c = 1; c <= 15; c++)
        ASSERT_EQ(parallel_fibonacci(25, c), 75025LL);
}

int main() {
    RUN_TEST("base cases 0 and 1", test_base_cases);
    RUN_TEST("small n=0..10", test_small);
    RUN_TEST("cutoff above n", test_cutoff_above_n);
    RUN_TEST("medium n=20", test_medium);
    RUN_TEST("large n=40", test_large);
    RUN_TEST("various cutoffs n=25", test_various_cutoffs);
    print_results();
    return 0;
}
