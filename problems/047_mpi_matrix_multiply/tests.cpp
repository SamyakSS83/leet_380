// tests.cpp for problem 047 — MPI Matrix Multiply
#include "harness.h"
#include <mpi.h>
#include <vector>
#include <cmath>

std::vector<double> distributed_matmul(
    const std::vector<double>& local_A, int local_rows,
    const std::vector<double>& B, int n, int k, int m);

// Sequential reference matmul
static std::vector<double> seq_matmul(const std::vector<double>& A, const std::vector<double>& B,
                                       int n, int k, int m) {
    std::vector<double> C(n * m, 0.0);
    for (int i = 0; i < n; i++)
        for (int p = 0; p < k; p++)
            for (int j = 0; j < m; j++)
                C[i*m+j] += A[i*k+p] * B[p*m+j];
    return C;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Test 1: identity * identity = identity (4x4)
    RUN_TEST("identity times identity", [&]() {
        int n = 4, k = 4, m = 4, local_rows = n / size;
        // Identity matrix
        std::vector<double> full_A(n*k, 0.0), B(k*m, 0.0);
        for (int i = 0; i < n; i++) full_A[i*k+i] = 1.0;
        for (int i = 0; i < k; i++) B[i*m+i] = 1.0;
        std::vector<double> local_A(local_rows * k);
        for (int r = 0; r < local_rows; r++)
            for (int c = 0; c < k; c++)
                local_A[r*k+c] = full_A[(rank*local_rows+r)*k+c];
        auto C = distributed_matmul(local_A, local_rows, B, n, k, m);
        if (rank == 0) {
            ASSERT_EQ((int)C.size(), n*m);
            auto expected = seq_matmul(full_A, B, n, k, m);
            ASSERT_VEC_APPROX(C, expected);
        }
    });

    // Test 2: zero matrix
    RUN_TEST("zero matrix", [&]() {
        int n = 4, k = 4, m = 4, local_rows = n / size;
        std::vector<double> full_A(n*k, 0.0), B(k*m, 1.0);
        std::vector<double> local_A(local_rows * k, 0.0);
        auto C = distributed_matmul(local_A, local_rows, B, n, k, m);
        if (rank == 0) {
            for (double v : C) ASSERT_APPROX(v, 0.0);
        }
    });

    // Test 3: ones matrix * ones = k * ones
    RUN_TEST("all-ones times all-ones", [&]() {
        int n = 4, k = 4, m = 4, local_rows = n / size;
        std::vector<double> full_A(n*k, 1.0), B(k*m, 1.0);
        std::vector<double> local_A(local_rows * k, 1.0);
        auto C = distributed_matmul(local_A, local_rows, B, n, k, m);
        if (rank == 0) {
            for (double v : C) ASSERT_APPROX(v, (double)k);
        }
    });

    // Test 4: random 8x6 * 6x5
    RUN_TEST("random 8x6 times 6x5", [&]() {
        int n = 8, k = 6, m = 5, local_rows = n / size;
        auto full_A = rand_vec(n * k, 42);
        auto B = rand_vec(k * m, 43);
        std::vector<double> local_A(local_rows * k);
        for (int r = 0; r < local_rows; r++)
            for (int c = 0; c < k; c++)
                local_A[r*k+c] = full_A[(rank*local_rows+r)*k+c];
        auto C = distributed_matmul(local_A, local_rows, B, n, k, m);
        if (rank == 0) {
            ASSERT_EQ((int)C.size(), n*m);
            auto expected = seq_matmul(full_A, B, n, k, m);
            for (int i = 0; i < n*m; i++)
                ASSERT_TRUE(fabs(C[i] - expected[i]) < 1e-6, "value mismatch");
        }
    });

    // Test 5: non-square, n=4 k=1 m=4 (outer product)
    RUN_TEST("outer product n=4 k=1 m=4", [&]() {
        int n = 4, k = 1, m = 4, local_rows = n / size;
        // A is column vector [1,2,3,4]^T, B is row vector [1,1,1,1]
        std::vector<double> full_A = {1,2,3,4};
        std::vector<double> B = {1,1,1,1};
        std::vector<double> local_A(local_rows * k);
        for (int r = 0; r < local_rows; r++) local_A[r] = full_A[rank*local_rows+r];
        auto C = distributed_matmul(local_A, local_rows, B, n, k, m);
        if (rank == 0) {
            // C[i][j] = (i+1)
            for (int i = 0; i < n; i++)
                for (int j = 0; j < m; j++)
                    ASSERT_APPROX(C[i*m+j], (double)(i+1));
        }
    });

    // Test 6: output size correct
    RUN_TEST("output size correct", [&]() {
        int n = 4, k = 3, m = 5, local_rows = n / size;
        std::vector<double> local_A(local_rows * k, 1.0);
        std::vector<double> B(k * m, 1.0);
        auto C = distributed_matmul(local_A, local_rows, B, n, k, m);
        if (rank == 0) ASSERT_EQ((int)C.size(), n * m);
    });

    // Test 7: non-rank-0 returns empty
    RUN_TEST("non-rank-0 empty output", [&]() {
        int n = 4, k = 4, m = 4, local_rows = n / size;
        std::vector<double> local_A(local_rows * k, 1.0);
        std::vector<double> B(k * m, 1.0);
        auto C = distributed_matmul(local_A, local_rows, B, n, k, m);
        if (rank != 0) ASSERT_EQ((int)C.size(), 0);
    });

    // Test 8: diagonal matrix multiply
    RUN_TEST("diagonal matrix multiply", [&]() {
        int n = 4, k = 4, m = 4, local_rows = n / size;
        // A = diag(1,2,3,4), B = all ones
        std::vector<double> full_A(n*k, 0.0);
        for (int i = 0; i < n; i++) full_A[i*k+i] = (double)(i+1);
        std::vector<double> B(k*m, 1.0);
        std::vector<double> local_A(local_rows * k);
        for (int r = 0; r < local_rows; r++)
            for (int c = 0; c < k; c++)
                local_A[r*k+c] = full_A[(rank*local_rows+r)*k+c];
        auto C = distributed_matmul(local_A, local_rows, B, n, k, m);
        if (rank == 0) {
            // C[i][j] = (i+1) for all j
            for (int i = 0; i < n; i++)
                for (int j = 0; j < m; j++)
                    ASSERT_APPROX(C[i*m+j], (double)(i+1));
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
