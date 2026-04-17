// tests.cpp for problem 027 — MPI Ping-Pong
#include "harness.h"
#include <mpi.h>

double mpi_ping_pong(int N, int msg_size);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double rtt_small = mpi_ping_pong(100, 1);
    double rtt_med   = mpi_ping_pong(50, 100);
    double rtt_large = mpi_ping_pong(10, 1000);

    RUN_TEST("small message RTT positive", [&]() {
        if (rank == 0) ASSERT_TRUE(rtt_small > 0.0, "RTT must be positive");
    });
    RUN_TEST("medium message RTT positive", [&]() {
        if (rank == 0) ASSERT_TRUE(rtt_med > 0.0, "RTT must be positive");
    });
    RUN_TEST("large message RTT positive", [&]() {
        if (rank == 0) ASSERT_TRUE(rtt_large > 0.0, "RTT must be positive");
    });
    RUN_TEST("non-rank-0 returns 0", [&]() {
        if (rank != 0) { ASSERT_APPROX(rtt_small, 0.0); }
    });
    RUN_TEST("large RTT >= small RTT", [&]() {
        if (rank == 0) ASSERT_TRUE(rtt_large >= rtt_small * 0.01,
            "large msg RTT should not be orders smaller");
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
