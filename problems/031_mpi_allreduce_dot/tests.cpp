// tests.cpp for problem 031 — MPI AllReduce Dot Product
#include "harness.h"
#include <mpi.h>

double mpi_allreduce_dot(const std::vector<double>& local_a,
                          const std::vector<double>& local_b);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("all ones dot product", [&]() {
        int chunk = 10;
        std::vector<double> a(chunk, 1.0), b(chunk, 1.0);
        double r = mpi_allreduce_dot(a, b);
        // All ranks get same result
        ASSERT_APPROX(r, (double)(chunk * size));
    });

    RUN_TEST("each rank contributes rank*rank", [&]() {
        std::vector<double> a = {(double)rank};
        std::vector<double> b = {(double)rank};
        double r = mpi_allreduce_dot(a, b);
        // sum of rank^2 for rank 0..size-1
        double expected = 0.0;
        for (int i = 0; i < size; i++) expected += (double)(i*i);
        ASSERT_APPROX(r, expected);
    });

    RUN_TEST("all ranks get same result", [&]() {
        std::vector<double> a = {(double)(rank+1)};
        std::vector<double> b = {2.0};
        double r1 = mpi_allreduce_dot(a, b);
        double r2 = mpi_allreduce_dot(a, b);
        ASSERT_APPROX(r1, r2);
    });

    RUN_TEST("zero vector", [&]() {
        std::vector<double> a(5, 0.0), b(5, 99.0);
        double r = mpi_allreduce_dot(a, b);
        ASSERT_APPROX(r, 0.0);
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
