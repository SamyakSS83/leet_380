// tests.cpp for problem 059 — Hybrid Stencil 1D
#include "harness.h"
#include <mpi.h>
#include <omp.h>
#include <vector>
#include <cmath>

std::vector<double> hybrid_stencil_1d(
    std::vector<double> local_u, int iterations);

// Serial reference: Jacobi stencil on full array with zero boundary ghosts
static std::vector<double> serial_stencil(std::vector<double> u, int iters) {
    // u is interior only; ghost = 0 on both ends
    int n = (int)u.size();
    std::vector<double> padded(n + 2, 0.0);
    for (int i = 0; i < n; i++) padded[i+1] = u[i];

    for (int it = 0; it < iters; it++) {
        std::vector<double> tmp(n + 2);
        tmp[0] = 0.0; tmp[n+1] = 0.0;
        for (int i = 1; i <= n; i++)
            tmp[i] = 0.5 * (padded[i-1] + padded[i+1]);
        padded = tmp;
    }
    return std::vector<double>(padded.begin()+1, padded.end()-1);
}

// Build local_u with ghost cells for a rank given the full interior array
static std::vector<double> build_local(const std::vector<double>& full,
                                        int rank, int size) {
    int total = (int)full.size();
    int chunk = total / size;
    int start = rank * chunk;
    // ghost left: 0 if rank==0, else full[start-1]
    // ghost right: 0 if rank==size-1, else full[start+chunk]
    std::vector<double> local(chunk + 2);
    local[0] = (rank == 0) ? 0.0 : full[start - 1];
    for (int i = 0; i < chunk; i++) local[i+1] = full[start + i];
    local[chunk+1] = (rank == size-1) ? 0.0 : full[start + chunk];
    return local;
}

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Total interior = 4 * size to distribute evenly
    int total = 4 * size;

    // Test 1: zero initial => stays zero for any iterations
    RUN_TEST("zero initial stays zero", [&]() {
        std::vector<double> full(total, 0.0);
        auto local = build_local(full, rank, size);
        auto result = hybrid_stencil_1d(local, 5);
        if (rank == 0) {
            for (double v : result) ASSERT_APPROX(v, 0.0);
        }
    });

    // Test 2: 0 iterations returns initial interior unchanged
    RUN_TEST("zero iterations returns input", [&]() {
        auto full = rand_vec(total, 10);
        auto local = build_local(full, rank, size);
        auto result = hybrid_stencil_1d(local, 0);
        if (rank == 0) {
            for (int i = 0; i < total; i++)
                ASSERT_APPROX(result[i], full[i]);
        }
    });

    // Test 3: 1 iteration matches serial reference
    RUN_TEST("1 iteration matches serial", [&]() {
        auto full = rand_vec(total, 42);
        auto local = build_local(full, rank, size);
        auto result = hybrid_stencil_1d(local, 1);
        if (rank == 0) {
            auto ref = serial_stencil(full, 1);
            ASSERT_VEC_APPROX(result, ref);
        }
    });

    // Test 4: 5 iterations matches serial reference
    RUN_TEST("5 iterations matches serial", [&]() {
        auto full = rand_vec(total, 99);
        auto local = build_local(full, rank, size);
        auto result = hybrid_stencil_1d(local, 5);
        if (rank == 0) {
            auto ref = serial_stencil(full, 5);
            ASSERT_VEC_APPROX(result, ref);
        }
    });

    // Test 5: 10 iterations matches serial reference
    RUN_TEST("10 iterations matches serial", [&]() {
        auto full = rand_vec(total, 7);
        auto local = build_local(full, rank, size);
        auto result = hybrid_stencil_1d(local, 10);
        if (rank == 0) {
            auto ref = serial_stencil(full, 10);
            ASSERT_VEC_APPROX(result, ref);
        }
    });

    // Test 6: constant interior => each step halves interior (not constant due to zero boundary)
    RUN_TEST("constant 2.0 interior result size correct", [&]() {
        std::vector<double> full(total, 2.0);
        auto local = build_local(full, rank, size);
        auto result = hybrid_stencil_1d(local, 3);
        if (rank == 0) {
            ASSERT_TRUE((int)result.size() == total, "wrong result size");
        }
    });

    // Test 7: all values in result are finite
    RUN_TEST("all result values are finite", [&]() {
        auto full = rand_vec(total, 55);
        auto local = build_local(full, rank, size);
        auto result = hybrid_stencil_1d(local, 20);
        if (rank == 0) {
            for (double v : result)
                ASSERT_TRUE(std::isfinite(v), "non-finite value in result");
        }
    });

    // Test 8: values decay toward zero with many iterations (Dirichlet BC)
    RUN_TEST("values decay toward 0 with 100 iterations", [&]() {
        std::vector<double> full(total, 1.0);
        auto local = build_local(full, rank, size);
        auto result = hybrid_stencil_1d(local, 100);
        if (rank == 0) {
            for (double v : result)
                ASSERT_TRUE(std::fabs(v) < 1.0, "value did not decay");
        }
    });

    // Test 9: 20 iterations matches serial reference with different seed
    RUN_TEST("20 iterations matches serial (seed=333)", [&]() {
        auto full = rand_vec(total, 333);
        auto local = build_local(full, rank, size);
        auto result = hybrid_stencil_1d(local, 20);
        if (rank == 0) {
            auto ref = serial_stencil(full, 20);
            ASSERT_VEC_APPROX(result, ref);
        }
    });

    // Test 10: result on non-rank-0 is empty
    RUN_TEST("non-rank-0 returns empty", [&]() {
        auto full = rand_vec(total, 1);
        auto local = build_local(full, rank, size);
        auto result = hybrid_stencil_1d(local, 3);
        if (rank != 0) ASSERT_TRUE(result.empty(), "non-rank-0 should return empty");
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
