// tests.cpp for problem 061 — Hybrid Word Count
#include "harness.h"
#include <mpi.h>
#include <omp.h>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>
#include <map>
#include <sstream>

std::vector<std::pair<std::string,int>> hybrid_word_count(const std::string& text);

// Serial reference word count
static std::vector<std::pair<std::string,int>> serial_wc(const std::string& text) {
    std::map<std::string,int> m;
    std::istringstream ss(text);
    std::string w;
    while (ss >> w) m[w]++;
    return std::vector<std::pair<std::string,int>>(m.begin(), m.end());
}

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    auto run = [&](const std::string& text) {
        return hybrid_word_count(rank == 0 ? text : "");
    };

    // Test 1: simple example
    RUN_TEST("simple cat sat mat example", [&]() {
        std::string text = "the cat sat on the mat the cat";
        auto result = run(text);
        if (rank == 0) {
            auto ref = serial_wc(text);
            ASSERT_TRUE(result == ref, "result mismatch");
        }
    });

    // Test 2: single word repeated
    RUN_TEST("single word repeated 10 times", [&]() {
        std::string text;
        for (int i = 0; i < 10; i++) text += (i ? " " : "") + std::string("hello");
        auto result = run(text);
        if (rank == 0) {
            ASSERT_TRUE(result.size() == 1, "should have 1 unique word");
            ASSERT_TRUE(result[0].first == "hello", "wrong word");
            ASSERT_TRUE(result[0].second == 10, "wrong count");
        }
    });

    // Test 3: empty text
    RUN_TEST("empty text returns empty result", [&]() {
        auto result = run("");
        if (rank == 0) ASSERT_TRUE(result.empty(), "empty text should give empty result");
    });

    // Test 4: all unique words
    RUN_TEST("all unique words each count 1", [&]() {
        std::string text = "alpha beta gamma delta epsilon zeta eta theta";
        auto result = run(text);
        if (rank == 0) {
            auto ref = serial_wc(text);
            ASSERT_TRUE(result == ref, "result mismatch");
            for (auto& p : result)
                ASSERT_TRUE(p.second == 1, "expected count 1");
        }
    });

    // Test 5: result is sorted lexicographically
    RUN_TEST("result is sorted by word", [&]() {
        std::string text = "zebra apple mango apple banana zebra apple";
        auto result = run(text);
        if (rank == 0) {
            for (size_t i = 1; i < result.size(); i++)
                ASSERT_TRUE(result[i-1].first < result[i].first, "not sorted");
        }
    });

    // Test 6: counts match serial reference
    RUN_TEST("counts match serial reference (medium text)", [&]() {
        std::string words[] = {"foo","bar","baz","qux","foo","bar","foo"};
        std::string text;
        for (int i = 0; i < 7; i++) text += (i ? " " : "") + words[i];
        auto result = run(text);
        if (rank == 0) {
            auto ref = serial_wc(text);
            ASSERT_TRUE(result == ref, "mismatch with serial reference");
        }
    });

    // Test 7: large repeated text
    RUN_TEST("large repeated text correct counts", [&]() {
        std::string chunk = "one two three four five ";
        std::string text;
        for (int i = 0; i < 200; i++) text += chunk;
        auto result = run(text);
        if (rank == 0) {
            auto ref = serial_wc(text);
            ASSERT_TRUE(result == ref, "large text mismatch");
        }
    });

    // Test 8: non-rank-0 returns empty
    RUN_TEST("non-rank-0 returns empty", [&]() {
        auto result = run("hello world");
        if (rank != 0) ASSERT_TRUE(result.empty(), "non-rank-0 should return empty");
    });

    // Test 9: single word text
    RUN_TEST("single word text", [&]() {
        auto result = run("onlyone");
        if (rank == 0) {
            ASSERT_TRUE(result.size() == 1, "should have 1 entry");
            ASSERT_TRUE(result[0].first == "onlyone", "wrong word");
            ASSERT_TRUE(result[0].second == 1, "wrong count");
        }
    });

    // Test 10: varied frequency text
    RUN_TEST("varied frequency counts match serial", [&]() {
        std::string text = "a a a b b c d d d d e";
        auto result = run(text);
        if (rank == 0) {
            auto ref = serial_wc(text);
            ASSERT_TRUE(result == ref, "varied frequency mismatch");
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
