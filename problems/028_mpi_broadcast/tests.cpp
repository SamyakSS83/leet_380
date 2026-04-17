// tests.cpp for problem 028 — MPI Broadcast
#include "harness.h"
#include <mpi.h>

void my_bcast(std::vector<double>& buf, int root);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("broadcast from rank 0 single value", [&]() {
        std::vector<double> buf(1, rank == 0 ? 42.0 : 0.0);
        my_bcast(buf, 0);
        ASSERT_APPROX(buf[0], 42.0);
    });

    RUN_TEST("broadcast array from rank 0", [&]() {
        std::vector<double> buf(10, rank == 0 ? 3.14 : 0.0);
        my_bcast(buf, 0);
        for (double x : buf) ASSERT_APPROX(x, 3.14);
    });

    RUN_TEST("all ranks agree after bcast", [&]() {
        std::vector<double> buf(5);
        if (rank == 0) for (int i = 0; i < 5; i++) buf[i] = i * 1.5;
        my_bcast(buf, 0);
        for (int i = 0; i < 5; i++) ASSERT_APPROX(buf[i], i * 1.5);
    });

    RUN_TEST("broadcast 100 elements", [&]() {
        std::vector<double> buf(100);
        if (rank == 0) for (int i = 0; i < 100; i++) buf[i] = (double)i;
        my_bcast(buf, 0);
        for (int i = 0; i < 100; i++) ASSERT_APPROX(buf[i], (double)i);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
