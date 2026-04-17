// tests.cpp for problem 022 — OMP Nested Parallelism
#include "harness.h"

double nested_parallel_sum(const std::vector<std::vector<double>>& mat);

void test_empty() { ASSERT_APPROX(nested_parallel_sum({}), 0.0); }

void test_1x1() { ASSERT_APPROX(nested_parallel_sum({{5.0}}), 5.0); }

void test_2x2() {
    ASSERT_APPROX(nested_parallel_sum({{1,2},{3,4}}), 10.0);
}

void test_all_ones() {
    std::vector<std::vector<double>> mat(10, std::vector<double>(10, 1.0));
    ASSERT_APPROX(nested_parallel_sum(mat), 100.0);
}

void test_row_of_negatives() {
    ASSERT_APPROX(nested_parallel_sum({{-1,-2,-3}}), -6.0);
}

void test_large() {
    int R = 100, C = 100;
    std::vector<std::vector<double>> mat(R, std::vector<double>(C, 1.0));
    ASSERT_APPROX(nested_parallel_sum(mat), (double)(R*C));
}

void test_asymmetric() {
    std::vector<std::vector<double>> mat = {{1,2,3,4,5},{6,7,8,9,10}};
    ASSERT_APPROX(nested_parallel_sum(mat), 55.0);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("1x1", test_1x1);
    RUN_TEST("2x2", test_2x2);
    RUN_TEST("10x10 all ones", test_all_ones);
    RUN_TEST("row of negatives", test_row_of_negatives);
    RUN_TEST("large 100x100", test_large);
    RUN_TEST("asymmetric 2x5", test_asymmetric);
    print_results();
    return 0;
}
