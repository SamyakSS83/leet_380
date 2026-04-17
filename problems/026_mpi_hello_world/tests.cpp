// tests.cpp for problem 026 — MPI Hello World
#include "harness.h"
#include <mpi.h>
#include <string>
#include <sstream>

std::vector<std::string> mpi_hello(int argc, char** argv);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    auto msgs = mpi_hello(argc, argv);

    RUN_TEST("rank 0 gets all messages", [&]() {
        if (rank == 0) {
            ASSERT_EQ((int)msgs.size(), size);
        }
    });

    RUN_TEST("messages contain rank numbers", [&]() {
        if (rank == 0) {
            for (int r = 0; r < size; r++) {
                bool found = false;
                std::string target = "rank " + std::to_string(r) + " of";
                for (auto& m : msgs) if (m.find(target) != std::string::npos) found = true;
                ASSERT_TRUE(found, ("missing message for rank " + std::to_string(r)).c_str());
            }
        }
    });

    RUN_TEST("messages are sorted", [&]() {
        if (rank == 0 && msgs.size() > 1) {
            for (int i = 1; i < (int)msgs.size(); i++)
                ASSERT_TRUE(msgs[i-1] <= msgs[i], "not sorted");
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
