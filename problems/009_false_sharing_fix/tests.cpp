// tests.cpp for problem 009 — False Sharing Fix
#include "harness.h"

double sum_with_false_sharing(const std::vector<double>& v, int nthreads);
double sum_with_padding(const std::vector<double>& v, int nthreads);

void test_false_sharing_basic() {
    double r = sum_with_false_sharing({1.0,2.0,3.0,4.0,5.0}, 4);
    ASSERT_APPROX(r, 15.0);
}

void test_padding_basic() {
    double r = sum_with_padding({1.0,2.0,3.0,4.0,5.0}, 4);
    ASSERT_APPROX(r, 15.0);
}

void test_both_agree() {
    auto v = rand_vec(10000, 42, 0.0, 1.0);
    double r1 = sum_with_false_sharing(v, 4);
    double r2 = sum_with_padding(v, 4);
    ASSERT_APPROX(r1, r2, 1e-6, 1e-6);
}

void test_empty_fs() {
    double r = sum_with_false_sharing({}, 4);
    ASSERT_APPROX(r, 0.0);
}

void test_empty_pad() {
    double r = sum_with_padding({}, 4);
    ASSERT_APPROX(r, 0.0);
}

void test_single_thread() {
    auto v = rand_vec(1000, 7, -1.0, 1.0);
    double r1 = sum_with_false_sharing(v, 1);
    double r2 = sum_with_padding(v, 1);
    double expected = 0.0;
    for (double x : v) expected += x;
    ASSERT_APPROX(r1, expected, 1e-6, 1e-6);
    ASSERT_APPROX(r2, expected, 1e-6, 1e-6);
}

void test_large() {
    auto v = rand_vec(1000000, 3, 0.0, 1.0);
    double r1 = sum_with_false_sharing(v, 4);
    double r2 = sum_with_padding(v, 4);
    ASSERT_APPROX(r1, r2, 1e-4, 1e-2);
}

int main() {
    RUN_TEST("false sharing basic", test_false_sharing_basic);
    RUN_TEST("padding basic", test_padding_basic);
    RUN_TEST("both agree 10k", test_both_agree);
    RUN_TEST("empty false sharing", test_empty_fs);
    RUN_TEST("empty padding", test_empty_pad);
    RUN_TEST("single thread", test_single_thread);
    RUN_TEST("large 1M", test_large);
    print_results();
    return 0;
}
