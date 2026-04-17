// tests.cpp for problem 060 — Hybrid Monte Carlo Pi
#include "harness.h"
#include <mpi.h>
#include <omp.h>
#include <cmath>

double hybrid_monte_carlo_pi(long long samples_per_thread);

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Test 1: large sample count close to pi
    RUN_TEST("1M samples/thread close to pi", [&]() {
        double pi = hybrid_monte_carlo_pi(1000000LL);
        if (rank == 0) ASSERT_TRUE(std::fabs(pi - 3.14159265) < 0.05,
                                   "pi estimate too far from true value");
    });

    // Test 2: result in valid range [0, 4]
    RUN_TEST("result in range [0, 4]", [&]() {
        double pi = hybrid_monte_carlo_pi(100LL);
        if (rank == 0) ASSERT_TRUE(pi >= 0.0 && pi <= 4.0, "result out of range [0,4]");
    });

    // Test 3: non-rank-0 returns 0
    RUN_TEST("non-rank-0 returns 0.0", [&]() {
        double pi = hybrid_monte_carlo_pi(1000LL);
        if (rank != 0) ASSERT_APPROX(pi, 0.0);
    });

    // Test 4: medium sample count within 0.1
    RUN_TEST("100k samples/thread within 0.1 of pi", [&]() {
        double pi = hybrid_monte_carlo_pi(100000LL);
        if (rank == 0) ASSERT_TRUE(std::fabs(pi - 3.14159265) < 0.1,
                                   "estimate not within 0.1 of pi");
    });

    // Test 5: deterministic (same samples_per_thread => same result)
    RUN_TEST("deterministic: same input same result", [&]() {
        double pi1 = hybrid_monte_carlo_pi(50000LL);
        double pi2 = hybrid_monte_carlo_pi(50000LL);
        if (rank == 0) ASSERT_APPROX(pi1, pi2);
    });

    // Test 6: more samples => better estimate (statistically expected)
    RUN_TEST("small sample still returns positive pi", [&]() {
        double pi = hybrid_monte_carlo_pi(10LL);
        if (rank == 0) ASSERT_TRUE(pi > 0.0, "pi estimate must be positive");
    });

    // Test 7: 500k samples within 0.05
    RUN_TEST("500k samples/thread within 0.05 of pi", [&]() {
        double pi = hybrid_monte_carlo_pi(500000LL);
        if (rank == 0) ASSERT_TRUE(std::fabs(pi - 3.14159265) < 0.05,
                                   "estimate not within 0.05 of pi");
    });

    // Test 8: result <= 4.0
    RUN_TEST("result never exceeds 4.0", [&]() {
        double pi = hybrid_monte_carlo_pi(1000LL);
        if (rank == 0) ASSERT_TRUE(pi <= 4.0, "result exceeds 4.0");
    });

    // Test 9: 1 sample per thread returns a multiple of 4/total_samples
    RUN_TEST("1 sample per thread returns a valid discrete value", [&]() {
        double pi = hybrid_monte_carlo_pi(1LL);
        if (rank == 0) ASSERT_TRUE(pi >= 0.0 && pi <= 4.0, "1-sample result out of range");
    });

    // Test 10: 2M samples within 0.02
    RUN_TEST("2M samples/thread within 0.02 of pi", [&]() {
        double pi = hybrid_monte_carlo_pi(2000000LL);
        if (rank == 0) ASSERT_TRUE(std::fabs(pi - 3.14159265) < 0.02,
                                   "estimate not within 0.02 of pi");
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
