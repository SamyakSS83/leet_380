// tests.cpp for problem 058 — Hybrid Matrix Multiply
#include "harness.h"
#include <mpi.h>
#include <omp.h>
#include <vector>
#include <cmath>

std::vector<double> hybrid_matmul(
    const std::vector<double>& local_A, int local_rows,
    const std::vector<double>& B, int n, int k, int m);

// Reference serial matmul for verification
static std::vector<double> serial_matmul(
    const std::vector<double>& A, int n, int k,
    const std::vector<double>& B, int m) {
    std::vector<double> C(n * m, 0.0);
    for (int i = 0; i < n; i++)
        for (int p = 0; p < k; p++)
            for (int j = 0; j < m; j++)
                C[i * m + j] += A[i * k + p] * B[p * m + j];
    return C;
}

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Helper: distribute full A row-wise and call hybrid_matmul
    auto run_test = [&](int n, int k, int m,
                        const std::vector<double>& A,
                        const std::vector<double>& B) -> std::vector<double> {
        // Distribute rows of A evenly (n must be divisible by size for simplicity)
        int base = n / size;
        int extra = n % size;
        int local_rows = base + (rank < extra ? 1 : 0);
        int row_start = rank * base + std::min(rank, extra);

        std::vector<double> local_A(local_rows * k);
        for (int i = 0; i < local_rows; i++)
            for (int j = 0; j < k; j++)
                local_A[i * k + j] = A[(row_start + i) * k + j];

        return hybrid_matmul(local_A, local_rows, B, n, k, m);
    };

    // Test 1: 4x4 identity * identity = identity
    RUN_TEST("4x4 identity times identity", [&]() {
        int n = 4, k = 4, m = 4;
        std::vector<double> I(n * k, 0.0);
        for (int i = 0; i < n; i++) I[i * k + i] = 1.0;
        auto C = run_test(n, k, m, I, I);
        if (rank == 0) ASSERT_VEC_APPROX(C, I);
    });

    // Test 2: 4x2 * 2x3 known result
    RUN_TEST("4x2 times 2x3 known result", [&]() {
        int n = 4, k = 2, m = 3;
        // A rows: [1,2],[3,4],[5,6],[7,8]
        std::vector<double> A = {1,2, 3,4, 5,6, 7,8};
        // B: [1,0,1],[0,1,0]
        std::vector<double> B = {1,0,1, 0,1,0};
        // Expected C: [1,2,1],[3,4,3],[5,6,5],[7,8,7]
        std::vector<double> expected = {1,2,1, 3,4,3, 5,6,5, 7,8,7};
        auto C = run_test(n, k, m, A, B);
        if (rank == 0) ASSERT_VEC_APPROX(C, expected);
    });

    // Test 3: 4x1 * 1x4 outer product
    RUN_TEST("4x1 times 1x4 outer product", [&]() {
        int n = 4, k = 1, m = 4;
        std::vector<double> A = {1,2,3,4};
        std::vector<double> B = {1,2,3,4};
        std::vector<double> expected = {
            1,2,3,4,
            2,4,6,8,
            3,6,9,12,
            4,8,12,16
        };
        auto C = run_test(n, k, m, A, B);
        if (rank == 0) ASSERT_VEC_APPROX(C, expected);
    });

    // Test 4: 4x4 zero matrix
    RUN_TEST("zero matrix times random is zero", [&]() {
        int n = 4, k = 4, m = 4;
        std::vector<double> A(n * k, 0.0);
        auto bv = rand_vec(k * m, 7);
        auto C = run_test(n, k, m, A, bv);
        if (rank == 0) {
            for (double v : C) ASSERT_APPROX(v, 0.0);
        }
    });

    // Test 5: compare with serial reference (random 8x6 * 6x5)
    RUN_TEST("8x6 times 6x5 matches serial reference", [&]() {
        int n = 8, k = 6, m = 5;
        auto A = rand_vec(n * k, 42);
        auto B = rand_vec(k * m, 99);
        auto C = run_test(n, k, m, A, B);
        if (rank == 0) {
            auto ref = serial_matmul(A, n, k, B, m);
            ASSERT_VEC_APPROX(C, ref);
        }
    });

    // Test 6: single column B (matrix-vector)
    RUN_TEST("4x4 matrix times 4x1 vector", [&]() {
        int n = 4, k = 4, m = 1;
        std::vector<double> A = {1,0,0,0, 0,2,0,0, 0,0,3,0, 0,0,0,4};
        std::vector<double> B = {1,2,3,4};
        std::vector<double> expected = {1,4,9,16};
        auto C = run_test(n, k, m, A, B);
        if (rank == 0) ASSERT_VEC_APPROX(C, expected);
    });

    // Test 7: result size correct (12x3 * 3x7)
    RUN_TEST("12x3 times 3x7 result has correct size", [&]() {
        int n = 12, k = 3, m = 7;
        auto A = rand_vec(n * k, 11);
        auto B = rand_vec(k * m, 22);
        auto C = run_test(n, k, m, A, B);
        if (rank == 0) {
            ASSERT_TRUE((int)C.size() == n * m, "result size mismatch");
        }
    });

    // Test 8: scaling — 2*A gives 2*C
    RUN_TEST("scaling: 2*A gives 2*C", [&]() {
        int n = 4, k = 4, m = 4;
        auto A = rand_vec(n * k, 55);
        auto B = rand_vec(k * m, 66);
        std::vector<double> A2(A.size());
        for (size_t i = 0; i < A.size(); i++) A2[i] = 2.0 * A[i];
        auto C1 = run_test(n, k, m, A, B);
        auto C2 = run_test(n, k, m, A2, B);
        if (rank == 0) {
            for (size_t i = 0; i < C1.size(); i++)
                ASSERT_APPROX(C2[i], 2.0 * C1[i]);
        }
    });

    // Test 9: random 16x8 * 8x4 matches serial
    RUN_TEST("16x8 times 8x4 matches serial reference", [&]() {
        int n = 16, k = 8, m = 4;
        auto A = rand_vec(n * k, 123);
        auto B = rand_vec(k * m, 456);
        auto C = run_test(n, k, m, A, B);
        if (rank == 0) {
            auto ref = serial_matmul(A, n, k, B, m);
            ASSERT_VEC_APPROX(C, ref);
        }
    });

    // Test 10: non-square A (4x8 * 8x2)
    RUN_TEST("4x8 times 8x2 matches serial reference", [&]() {
        int n = 4, k = 8, m = 2;
        auto A = rand_vec(n * k, 77);
        auto B = rand_vec(k * m, 88);
        auto C = run_test(n, k, m, A, B);
        if (rank == 0) {
            auto ref = serial_matmul(A, n, k, B, m);
            ASSERT_VEC_APPROX(C, ref);
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
