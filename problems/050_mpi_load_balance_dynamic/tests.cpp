// tests.cpp for problem 050 — MPI Load Balance Dynamic
#include "harness.h"
#include <mpi.h>
#include <vector>
#include <numeric>

std::vector<long long> dynamic_load_balance(int num_tasks);

// Reference fib for validation
static long long ref_fib(int n) {
    if (n <= 1) return n;
    long long a = 0, b = 1;
    for (int i = 2; i <= n; i++) { long long c = a + b; a = b; b = c; }
    return b;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    RUN_TEST("single task", [&]() {
        auto r = dynamic_load_balance(1);
        if (rank == 0) {
            ASSERT_EQ((int)r.size(), 1);
            ASSERT_EQ(r[0], ref_fib(1));
        } else {
            ASSERT_TRUE(r.empty(), "non-rank-0 should return empty");
        }
    });

    RUN_TEST("3 tasks results correct", [&]() {
        auto r = dynamic_load_balance(3);
        if (rank == 0) {
            ASSERT_EQ((int)r.size(), 3);
            for (int i = 0; i < 3; i++)
                ASSERT_EQ(r[i], ref_fib(i % 35 + 1));
        }
    });

    RUN_TEST("10 tasks results correct", [&]() {
        auto r = dynamic_load_balance(10);
        if (rank == 0) {
            ASSERT_EQ((int)r.size(), 10);
            for (int i = 0; i < 10; i++)
                ASSERT_EQ(r[i], ref_fib(i % 35 + 1));
        }
    });

    RUN_TEST("num_tasks equals num_workers", [&]() {
        // With 4 ranks, 3 workers — test exactly 3 tasks
        auto r = dynamic_load_balance(3);
        if (rank == 0) {
            ASSERT_EQ((int)r.size(), 3);
            for (int i = 0; i < 3; i++)
                ASSERT_EQ(r[i], ref_fib(i % 35 + 1));
        }
    });

    RUN_TEST("20 tasks all correct", [&]() {
        auto r = dynamic_load_balance(20);
        if (rank == 0) {
            ASSERT_EQ((int)r.size(), 20);
            for (int i = 0; i < 20; i++)
                ASSERT_EQ(r[i], ref_fib(i % 35 + 1));
        }
    });

    RUN_TEST("35 tasks wraps fib index", [&]() {
        auto r = dynamic_load_balance(35);
        if (rank == 0) {
            ASSERT_EQ((int)r.size(), 35);
            // index 34: fib(34 % 35 + 1) = fib(35)
            ASSERT_EQ(r[34], ref_fib(35));
        }
    });

    RUN_TEST("36 tasks modulo wrap", [&]() {
        auto r = dynamic_load_balance(36);
        if (rank == 0) {
            ASSERT_EQ((int)r.size(), 36);
            // index 35: fib(35 % 35 + 1) = fib(1) = 1
            ASSERT_EQ(r[35], ref_fib(1));
        }
    });

    RUN_TEST("50 tasks correctness", [&]() {
        auto r = dynamic_load_balance(50);
        if (rank == 0) {
            ASSERT_EQ((int)r.size(), 50);
            for (int i = 0; i < 50; i++)
                ASSERT_EQ(r[i], ref_fib(i % 35 + 1));
        }
    });

    RUN_TEST("non-rank-0 returns empty for 10 tasks", [&]() {
        auto r = dynamic_load_balance(10);
        if (rank != 0) ASSERT_TRUE(r.empty(), "workers must return empty vector");
    });

    RUN_TEST("100 tasks large run", [&]() {
        auto r = dynamic_load_balance(100);
        if (rank == 0) {
            ASSERT_EQ((int)r.size(), 100);
            for (int i = 0; i < 100; i++)
                ASSERT_EQ(r[i], ref_fib(i % 35 + 1));
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
