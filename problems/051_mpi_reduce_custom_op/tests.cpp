// tests.cpp for problem 051 — MPI Reduce Custom Op
#include "harness.h"
#include <mpi.h>

// ArgMax defined in solve.cpp
ArgMax mpi_argmax(double local_value);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("rank 0 has max", [&]() {
        // Give rank 0 value 100, others 1
        double val = (rank == 0) ? 100.0 : 1.0;
        ArgMax r = mpi_argmax(val);
        if (rank == 0) {
            ASSERT_APPROX(r.value, 100.0);
            ASSERT_EQ(r.rank, 0);
        }
    });

    RUN_TEST("last rank has max", [&]() {
        // Give last rank the largest value
        double val = (rank == size - 1) ? 999.0 : 0.0;
        ArgMax r = mpi_argmax(val);
        if (rank == 0) {
            ASSERT_APPROX(r.value, 999.0);
            ASSERT_EQ(r.rank, size - 1);
        }
    });

    RUN_TEST("all same value — lower rank wins", [&]() {
        ArgMax r = mpi_argmax(5.0);
        if (rank == 0) {
            ASSERT_APPROX(r.value, 5.0);
            ASSERT_EQ(r.rank, 0);
        }
    });

    RUN_TEST("negative values rank 2 wins", [&]() {
        // rank 2 has -1, others have -10
        double val = (rank == 2) ? -1.0 : -10.0;
        ArgMax r = mpi_argmax(val);
        if (rank == 0) {
            ASSERT_APPROX(r.value, -1.0);
            ASSERT_EQ(r.rank, 2);
        }
    });

    RUN_TEST("rank 1 has max", [&]() {
        double val = (rank == 1) ? 42.5 : -1.0;
        ArgMax r = mpi_argmax(val);
        if (rank == 0) {
            ASSERT_APPROX(r.value, 42.5);
            ASSERT_EQ(r.rank, 1);
        }
    });

    RUN_TEST("each rank unique increasing — last rank wins", [&]() {
        double val = (double)rank * 10.0;
        ArgMax r = mpi_argmax(val);
        if (rank == 0) {
            ASSERT_APPROX(r.value, (double)(size - 1) * 10.0);
            ASSERT_EQ(r.rank, size - 1);
        }
    });

    RUN_TEST("each rank unique decreasing — rank 0 wins", [&]() {
        double val = (double)(size - 1 - rank) * 7.0;
        ArgMax r = mpi_argmax(val);
        if (rank == 0) {
            ASSERT_APPROX(r.value, (double)(size - 1) * 7.0);
            ASSERT_EQ(r.rank, 0);
        }
    });

    RUN_TEST("zero and positive", [&]() {
        double val = (rank == 3 && size > 3) ? 0.001 : 0.0;
        ArgMax r = mpi_argmax(val);
        if (rank == 0) {
            if (size > 3) {
                ASSERT_APPROX(r.value, 0.001);
                ASSERT_EQ(r.rank, 3);
            } else {
                ASSERT_APPROX(r.value, 0.0);
                ASSERT_EQ(r.rank, 0);
            }
        }
    });

    RUN_TEST("fractional values", [&]() {
        double val = 1.0 / (rank + 1);  // rank 0 gets 1.0, highest
        ArgMax r = mpi_argmax(val);
        if (rank == 0) {
            ASSERT_APPROX(r.value, 1.0);
            ASSERT_EQ(r.rank, 0);
        }
    });

    RUN_TEST("large equal except rank 2", [&]() {
        double val = (rank == 2) ? 1e9 : 1.0;
        ArgMax r = mpi_argmax(val);
        if (rank == 0) {
            ASSERT_APPROX(r.value, 1e9);
            ASSERT_EQ(r.rank, 2);
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
