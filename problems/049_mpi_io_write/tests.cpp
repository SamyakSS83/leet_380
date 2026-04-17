// tests.cpp for problem 049 — MPI Parallel File I/O
#include "harness.h"
#include <mpi.h>
#include <vector>
#include <cstdio>
#include <cmath>
#include <string>

void parallel_file_write(const std::vector<double>& local_data, const char* filename);

// Read back file on rank 0 and return data
static std::vector<double> read_file(const char* filename, int total) {
    std::vector<double> data(total);
    FILE* f = fopen(filename, "rb");
    if (!f) return {};
    fread(data.data(), sizeof(double), total, f);
    fclose(f);
    return data;
}

static void remove_file(const char* filename) {
    std::remove(filename);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Test 1: each rank writes its rank value, verify order
    RUN_TEST("rank values written in order", [&]() {
        const char* fname = "/tmp/mpi_io_test1.bin";
        std::vector<double> local(4, (double)rank);
        parallel_file_write(local, fname);
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            auto data = read_file(fname, size * 4);
            ASSERT_EQ((int)data.size(), size * 4);
            for (int r = 0; r < size; r++)
                for (int i = 0; i < 4; i++)
                    ASSERT_APPROX(data[r*4+i], (double)r);
            remove_file(fname);
        }
    });

    // Test 2: sequential integers across all ranks
    RUN_TEST("sequential integers", [&]() {
        const char* fname = "/tmp/mpi_io_test2.bin";
        int n = 5;
        std::vector<double> local(n);
        for (int i = 0; i < n; i++) local[i] = (double)(rank * n + i);
        parallel_file_write(local, fname);
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            auto data = read_file(fname, size * n);
            for (int i = 0; i < size * n; i++)
                ASSERT_APPROX(data[i], (double)i);
            remove_file(fname);
        }
    });

    // Test 3: single element per rank
    RUN_TEST("single element per rank", [&]() {
        const char* fname = "/tmp/mpi_io_test3.bin";
        std::vector<double> local = {(double)(rank * rank)};
        parallel_file_write(local, fname);
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            auto data = read_file(fname, size);
            ASSERT_EQ((int)data.size(), size);
            for (int r = 0; r < size; r++)
                ASSERT_APPROX(data[r], (double)(r * r));
            remove_file(fname);
        }
    });

    // Test 4: file size is correct
    RUN_TEST("file size correct", [&]() {
        const char* fname = "/tmp/mpi_io_test4.bin";
        int n = 10;
        std::vector<double> local(n, 1.0);
        parallel_file_write(local, fname);
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            FILE* f = fopen(fname, "rb");
            ASSERT_TRUE(f != nullptr, "file not created");
            fseek(f, 0, SEEK_END);
            long sz = ftell(f);
            fclose(f);
            ASSERT_EQ((long)sz, (long)(size * n * sizeof(double)));
            remove_file(fname);
        }
    });

    // Test 5: zero values
    RUN_TEST("all zeros", [&]() {
        const char* fname = "/tmp/mpi_io_test5.bin";
        std::vector<double> local(8, 0.0);
        parallel_file_write(local, fname);
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            auto data = read_file(fname, size * 8);
            for (double v : data) ASSERT_APPROX(v, 0.0);
            remove_file(fname);
        }
    });

    // Test 6: floating point data
    RUN_TEST("floating point data", [&]() {
        const char* fname = "/tmp/mpi_io_test6.bin";
        auto local = rand_vec(6, 42 + rank);
        std::vector<double> all_local = local;
        parallel_file_write(local, fname);
        MPI_Barrier(MPI_COMM_WORLD);
        // Gather expected data
        std::vector<double> expected(size * 6);
        MPI_Gather(all_local.data(), 6, MPI_DOUBLE, expected.data(), 6, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if (rank == 0) {
            auto data = read_file(fname, size * 6);
            for (int i = 0; i < size * 6; i++)
                ASSERT_TRUE(fabs(data[i] - expected[i]) < 1e-12, "fp value mismatch");
            remove_file(fname);
        }
    });

    // Test 7: overwrite existing file
    RUN_TEST("overwrite existing file", [&]() {
        const char* fname = "/tmp/mpi_io_test7.bin";
        // Write once
        std::vector<double> local1(3, 1.0);
        parallel_file_write(local1, fname);
        MPI_Barrier(MPI_COMM_WORLD);
        // Write again with different data
        std::vector<double> local2(3, 2.0);
        parallel_file_write(local2, fname);
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            auto data = read_file(fname, size * 3);
            for (double v : data) ASSERT_APPROX(v, 2.0);
            remove_file(fname);
        }
    });

    // Test 8: large write
    RUN_TEST("large write 1000 elements per rank", [&]() {
        const char* fname = "/tmp/mpi_io_test8.bin";
        int n = 1000;
        std::vector<double> local(n, (double)rank);
        parallel_file_write(local, fname);
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) {
            auto data = read_file(fname, size * n);
            ASSERT_EQ((int)data.size(), size * n);
            for (int r = 0; r < size; r++)
                for (int i = 0; i < n; i++)
                    ASSERT_APPROX(data[r*n+i], (double)r);
            remove_file(fname);
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
