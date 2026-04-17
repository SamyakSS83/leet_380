// tests.cpp for problem 036 — MPI Derived Struct
#include "harness.h"
#include <mpi.h>

// Particle defined in solve.cpp
std::vector<Particle> mpi_send_particles(std::vector<Particle> particles);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    RUN_TEST("all ranks get correct particles", [&]() {
        std::vector<Particle> p;
        if (rank == 0) p = {{1.0,2.0,3.0,10},{4.0,5.0,6.0,20}};
        auto r = mpi_send_particles(p);
        ASSERT_EQ((int)r.size(), 2);
        ASSERT_APPROX(r[0].x, 1.0); ASSERT_EQ(r[0].id, 10);
        ASSERT_APPROX(r[1].z, 6.0); ASSERT_EQ(r[1].id, 20);
    });

    RUN_TEST("empty array", [&]() {
        std::vector<Particle> p;
        auto r = mpi_send_particles(p);
        ASSERT_TRUE(r.empty(), "expected empty");
    });

    RUN_TEST("single particle", [&]() {
        std::vector<Particle> p;
        if (rank == 0) p = {{3.14, 2.71, 1.41, 42}};
        auto r = mpi_send_particles(p);
        ASSERT_EQ((int)r.size(), 1);
        ASSERT_APPROX(r[0].x, 3.14);
        ASSERT_EQ(r[0].id, 42);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
