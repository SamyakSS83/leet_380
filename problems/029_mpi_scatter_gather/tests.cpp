// tests.cpp for problem 029 — MPI Scatter/Gather
#include "harness.h"
#include <mpi.h>

std::vector<double> scatter_square_gather(std::vector<double> data);

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    RUN_TEST("square 1..n", [&]() {
        std::vector<double> data;
        if (rank == 0) {
            data.resize(size * 4);
            for (int i = 0; i < (int)data.size(); i++) data[i] = (double)(i+1);
        }
        auto r = scatter_square_gather(data);
        if (rank == 0) {
            for (int i = 0; i < (int)r.size(); i++)
                ASSERT_APPROX(r[i], (double)((i+1)*(i+1)));
        }
    });

    RUN_TEST("all zeros", [&]() {
        std::vector<double> data;
        if (rank == 0) data.assign(size * 4, 0.0);
        auto r = scatter_square_gather(data);
        if (rank == 0) for (double x : r) ASSERT_APPROX(x, 0.0);
    });

    RUN_TEST("single element per rank", [&]() {
        std::vector<double> data;
        if (rank == 0) {
            data.resize(size);
            for (int i = 0; i < size; i++) data[i] = (double)(i+2);
        }
        auto r = scatter_square_gather(data);
        if (rank == 0)
            for (int i = 0; i < size; i++)
                ASSERT_APPROX(r[i], (double)((i+2)*(i+2)));
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
