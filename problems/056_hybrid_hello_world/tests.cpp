// tests.cpp for problem 056 — Hybrid Hello World
#include "harness.h"
#include <mpi.h>
#include <omp.h>
#include <vector>
#include <string>
#include <algorithm>

std::vector<std::string> hybrid_hello(int num_threads);

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("1 thread: total messages = size", [&]() {
        auto msgs = hybrid_hello(1);
        if (rank == 0) ASSERT_EQ((int)msgs.size(), size * 1);
    });

    RUN_TEST("2 threads: total messages = 2*size", [&]() {
        auto msgs = hybrid_hello(2);
        if (rank == 0) ASSERT_EQ((int)msgs.size(), size * 2);
    });

    RUN_TEST("non-rank-0 returns empty (1 thread)", [&]() {
        auto msgs = hybrid_hello(1);
        if (rank != 0) ASSERT_TRUE(msgs.empty(), "should be empty on non-root");
    });

    RUN_TEST("non-rank-0 returns empty (2 threads)", [&]() {
        auto msgs = hybrid_hello(2);
        if (rank != 0) ASSERT_TRUE(msgs.empty(), "should be empty on non-root");
    });

    RUN_TEST("messages are sorted (1 thread)", [&]() {
        auto msgs = hybrid_hello(1);
        if (rank == 0) {
            auto sorted = msgs;
            std::sort(sorted.begin(), sorted.end());
            ASSERT_TRUE(msgs == sorted, "messages must be sorted");
        }
    });

    RUN_TEST("messages are sorted (2 threads)", [&]() {
        auto msgs = hybrid_hello(2);
        if (rank == 0) {
            auto sorted = msgs;
            std::sort(sorted.begin(), sorted.end());
            ASSERT_TRUE(msgs == sorted, "messages must be sorted");
        }
    });

    RUN_TEST("rank 0 thread 0 message present (1 thread)", [&]() {
        auto msgs = hybrid_hello(1);
        if (rank == 0) {
            std::string expected = "Hello from rank 0 thread 0 of "
                + std::to_string(size) + " ranks 1 threads";
            bool found = std::find(msgs.begin(), msgs.end(), expected) != msgs.end();
            ASSERT_TRUE(found, "rank 0 thread 0 message not found");
        }
    });

    RUN_TEST("all ranks represented (1 thread)", [&]() {
        auto msgs = hybrid_hello(1);
        if (rank == 0) {
            for (int r = 0; r < size; r++) {
                std::string expected = "Hello from rank " + std::to_string(r)
                    + " thread 0 of " + std::to_string(size) + " ranks 1 threads";
                bool found = std::find(msgs.begin(), msgs.end(), expected) != msgs.end();
                ASSERT_TRUE(found, ("missing message for rank " + std::to_string(r)).c_str());
            }
        }
    });

    RUN_TEST("no duplicate messages (1 thread)", [&]() {
        auto msgs = hybrid_hello(1);
        if (rank == 0) {
            auto copy = msgs;
            std::sort(copy.begin(), copy.end());
            auto it = std::unique(copy.begin(), copy.end());
            ASSERT_EQ((int)(it - copy.begin()), (int)copy.size());
        }
    });

    RUN_TEST("3 threads message count correct", [&]() {
        auto msgs = hybrid_hello(3);
        if (rank == 0) ASSERT_EQ((int)msgs.size(), size * 3);
    });

    RUN_TEST("thread 1 message present with 2 threads", [&]() {
        auto msgs = hybrid_hello(2);
        if (rank == 0) {
            std::string expected = "Hello from rank 0 thread 1 of "
                + std::to_string(size) + " ranks 2 threads";
            bool found = std::find(msgs.begin(), msgs.end(), expected) != msgs.end();
            ASSERT_TRUE(found, "rank 0 thread 1 message not found");
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
