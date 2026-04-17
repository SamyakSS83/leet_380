// tests.cpp for problem 005 — Monte Carlo Pi (OpenMP)
#include "harness.h"
#include <cmath>

double monte_carlo_pi(long long n, unsigned seed);

void test_basic_convergence() {
    double pi = monte_carlo_pi(1000000, 42);
    ASSERT_TRUE(fabs(pi - 3.14159265) < 0.05, "pi estimate not within 0.05");
}

void test_small_n() {
    double pi = monte_carlo_pi(100, 1);
    ASSERT_TRUE(pi >= 0.0 && pi <= 4.0, "pi estimate must be in [0,4]");
}

void test_large_n() {
    double pi = monte_carlo_pi(10000000, 7);
    ASSERT_TRUE(fabs(pi - 3.14159265) < 0.01, "large n should be within 0.01");
}

void test_reproducible() {
    double p1 = monte_carlo_pi(500000, 123);
    double p2 = monte_carlo_pi(500000, 123);
    // With same seed, results should be identical (or very close)
    ASSERT_TRUE(fabs(p1 - p2) < 0.001, "same seed should give same result");
}

void test_different_seeds() {
    double p1 = monte_carlo_pi(100000, 1);
    double p2 = monte_carlo_pi(100000, 999);
    // Both should be near pi
    ASSERT_TRUE(fabs(p1 - 3.14159) < 0.05, "seed 1 estimate off");
    ASSERT_TRUE(fabs(p2 - 3.14159) < 0.05, "seed 999 estimate off");
}

void test_n1() {
    double pi = monte_carlo_pi(1, 42);
    ASSERT_TRUE(pi == 0.0 || pi == 4.0, "n=1 must give 0 or 4");
}

int main() {
    RUN_TEST("basic convergence 1M", test_basic_convergence);
    RUN_TEST("small n in range", test_small_n);
    RUN_TEST("large n 10M", test_large_n);
    RUN_TEST("reproducible same seed", test_reproducible);
    RUN_TEST("different seeds both near pi", test_different_seeds);
    RUN_TEST("n=1 gives 0 or 4", test_n1);
    print_results();
    return 0;
}
