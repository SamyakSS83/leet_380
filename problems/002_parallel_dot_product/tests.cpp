// tests.cpp for problem 002 — Parallel Dot Product
#include "harness.h"

double parallel_dot_product(const std::vector<double>& a, const std::vector<double>& b);

void test_empty() {
    ASSERT_APPROX(parallel_dot_product({}, {}), 0.0);
}

void test_single() {
    ASSERT_APPROX(parallel_dot_product({3.0}, {4.0}), 12.0);
}

void test_basic() {
    ASSERT_APPROX(parallel_dot_product({1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}), 32.0);
}

void test_orthogonal() {
    ASSERT_APPROX(parallel_dot_product({1.0, 0.0}, {0.0, 1.0}), 0.0);
}

void test_same_vector() {
    std::vector<double> v = {3.0, 4.0};
    ASSERT_APPROX(parallel_dot_product(v, v), 25.0);
}

void test_negative() {
    ASSERT_APPROX(parallel_dot_product({-1.0, 2.0, -3.0}, {4.0, -5.0, 6.0}), -32.0);
}

void test_large() {
    auto a = rand_vec(100000, 42, 0.0, 1.0);
    auto b = rand_vec(100000, 99, 0.0, 1.0);
    double expected = 0.0;
    for (int i = 0; i < 100000; i++) expected += a[i] * b[i];
    ASSERT_APPROX(parallel_dot_product(a, b), expected, 1e-4, 1e-2);
}

void test_unit_vectors() {
    std::vector<double> a(100, 1.0/10.0), b(100, 1.0/10.0);
    double expected = 1.0;
    ASSERT_APPROX(parallel_dot_product(a, b), expected, 1e-6, 1e-8);
}

void test_large_values() {
    ASSERT_APPROX(parallel_dot_product({1e6, 2e6}, {3e6, 4e6}), 11e12);
}

void test_million() {
    auto a = rand_vec(1000000, 1, -1.0, 1.0);
    auto b = rand_vec(1000000, 2, -1.0, 1.0);
    double expected = 0.0;
    for (int i = 0; i < 1000000; i++) expected += a[i]*b[i];
    ASSERT_APPROX(parallel_dot_product(a, b), expected, 1e-3, 1e-1);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("single", test_single);
    RUN_TEST("basic 1..3 dot 4..6", test_basic);
    RUN_TEST("orthogonal vectors", test_orthogonal);
    RUN_TEST("self dot product", test_same_vector);
    RUN_TEST("negative values", test_negative);
    RUN_TEST("large 100k", test_large);
    RUN_TEST("unit vectors", test_unit_vectors);
    RUN_TEST("large values", test_large_values);
    RUN_TEST("1M elements", test_million);
    print_results();
    return 0;
}
