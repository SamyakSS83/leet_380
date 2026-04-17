// tests.cpp for problem 053 — MPI Broadcast Tree
#include "harness.h"
#include <mpi.h>
#include <vector>

void tree_broadcast(std::vector<int>& data, int root);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("broadcast from root 0 single element", [&]() {
        std::vector<int> data = (rank == 0) ? std::vector<int>{42} : std::vector<int>{};
        tree_broadcast(data, 0);
        ASSERT_EQ((int)data.size(), 1);
        ASSERT_EQ(data[0], 42);
    });

    RUN_TEST("broadcast from root 0 multiple elements", [&]() {
        std::vector<int> data = (rank == 0) ? std::vector<int>{1, 2, 3, 4, 5} : std::vector<int>{};
        tree_broadcast(data, 0);
        ASSERT_EQ((int)data.size(), 5);
        ASSERT_EQ(data[0], 1);
        ASSERT_EQ(data[4], 5);
    });

    RUN_TEST("broadcast from root 1", [&]() {
        std::vector<int> data = (rank == 1) ? std::vector<int>{100, 200, 300} : std::vector<int>{};
        tree_broadcast(data, 1);
        ASSERT_EQ((int)data.size(), 3);
        ASSERT_EQ(data[0], 100);
        ASSERT_EQ(data[2], 300);
    });

    RUN_TEST("broadcast from last rank", [&]() {
        std::vector<int> data = (rank == size - 1) ? std::vector<int>{-7, -8} : std::vector<int>{};
        tree_broadcast(data, size - 1);
        ASSERT_EQ((int)data.size(), 2);
        ASSERT_EQ(data[0], -7);
        ASSERT_EQ(data[1], -8);
    });

    RUN_TEST("all ranks agree after broadcast from 0", [&]() {
        std::vector<int> data = (rank == 0) ? std::vector<int>{10, 20, 30} : std::vector<int>{};
        tree_broadcast(data, 0);
        // Verify by gathering sums
        int local_sum = data[0] + data[1] + data[2];
        int global_sum = 0;
        MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if (rank == 0) ASSERT_EQ(global_sum, 60 * size);
    });

    RUN_TEST("large vector broadcast", [&]() {
        std::vector<int> data;
        if (rank == 0) {
            data.resize(1000);
            for (int i = 0; i < 1000; i++) data[i] = i * 3;
        }
        tree_broadcast(data, 0);
        ASSERT_EQ((int)data.size(), 1000);
        ASSERT_EQ(data[0], 0);
        ASSERT_EQ(data[999], 999 * 3);
    });

    RUN_TEST("broadcast from root 2", [&]() {
        std::vector<int> data = (rank == 2 && size > 2) ? std::vector<int>{55, 66} : std::vector<int>{};
        int root = (size > 2) ? 2 : 0;
        if (rank == 0 && size <= 2) data = {55, 66};
        tree_broadcast(data, root);
        ASSERT_EQ((int)data.size(), 2);
        ASSERT_EQ(data[0], 55);
        ASSERT_EQ(data[1], 66);
    });

    RUN_TEST("all values preserved exactly", [&]() {
        std::vector<int> ref = {-100, 0, 1, 999, -1};
        std::vector<int> data = (rank == 0) ? ref : std::vector<int>{};
        tree_broadcast(data, 0);
        ASSERT_VEC_EQ(data, ref);
    });

    RUN_TEST("broadcast root 0 then verify on rank 3", [&]() {
        std::vector<int> data = (rank == 0) ? std::vector<int>{7, 14, 21} : std::vector<int>{};
        tree_broadcast(data, 0);
        if (rank == 3 && size > 3) {
            ASSERT_EQ(data[0], 7);
            ASSERT_EQ(data[1], 14);
            ASSERT_EQ(data[2], 21);
        }
    });

    RUN_TEST("repeated broadcasts consistent", [&]() {
        for (int root = 0; root < std::min(size, 3); root++) {
            std::vector<int> data = (rank == root) ? std::vector<int>{root * 10, root * 10 + 1} : std::vector<int>{};
            tree_broadcast(data, root);
            ASSERT_EQ(data[0], root * 10);
            ASSERT_EQ(data[1], root * 10 + 1);
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
