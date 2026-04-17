// tests.cpp for problem 046 — MPI Word Count
#include "harness.h"
#include <mpi.h>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

std::vector<std::pair<std::string, int>> mpi_word_count(const std::string& text);

static std::vector<std::pair<std::string,int>> make_wc(std::initializer_list<std::pair<std::string,int>> lst) {
    std::vector<std::pair<std::string,int>> v(lst);
    std::sort(v.begin(), v.end());
    return v;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Test 1: simple repeated words
    RUN_TEST("simple repeated words", [&]() {
        std::string text = (rank == 0) ? "a b a c b a" : "";
        auto result = mpi_word_count(text);
        if (rank == 0) {
            auto expected = make_wc({{"a",3},{"b",2},{"c",1}});
            ASSERT_EQ((int)result.size(), 3);
            for (int i = 0; i < (int)result.size(); i++) {
                ASSERT_EQ(result[i].first, expected[i].first);
                ASSERT_EQ(result[i].second, expected[i].second);
            }
        }
    });

    // Test 2: all same word
    RUN_TEST("all same word", [&]() {
        std::string text = (rank == 0) ? "hello hello hello hello hello hello hello hello" : "";
        auto result = mpi_word_count(text);
        if (rank == 0) {
            ASSERT_EQ((int)result.size(), 1);
            ASSERT_EQ(result[0].first, std::string("hello"));
            ASSERT_EQ(result[0].second, 8);
        }
    });

    // Test 3: single word
    RUN_TEST("single word", [&]() {
        std::string text = (rank == 0) ? "only" : "";
        auto result = mpi_word_count(text);
        if (rank == 0) {
            ASSERT_EQ((int)result.size(), 1);
            ASSERT_EQ(result[0].first, std::string("only"));
            ASSERT_EQ(result[0].second, 1);
        }
    });

    // Test 4: result is sorted lexicographically
    RUN_TEST("result sorted lexicographically", [&]() {
        std::string text = (rank == 0) ? "zebra apple mango apple zebra banana apple" : "";
        auto result = mpi_word_count(text);
        if (rank == 0) {
            ASSERT_TRUE((int)result.size() == 4, "wrong number of unique words");
            for (int i = 0; i + 1 < (int)result.size(); i++)
                ASSERT_TRUE(result[i].first < result[i+1].first, "not sorted");
        }
    });

    // Test 5: correct counts in sorted output
    RUN_TEST("correct counts apple mango zebra banana", [&]() {
        std::string text = (rank == 0) ? "zebra apple mango apple zebra banana apple" : "";
        auto result = mpi_word_count(text);
        if (rank == 0) {
            // sorted: apple=3, banana=1, mango=1, zebra=2
            auto expected = make_wc({{"apple",3},{"banana",1},{"mango",1},{"zebra",2}});
            ASSERT_EQ((int)result.size(), (int)expected.size());
            for (int i = 0; i < (int)result.size(); i++) {
                ASSERT_EQ(result[i].first, expected[i].first);
                ASSERT_EQ(result[i].second, expected[i].second);
            }
        }
    });

    // Test 6: non-rank-0 returns empty
    RUN_TEST("non-rank-0 returns empty", [&]() {
        std::string text = (rank == 0) ? "foo bar foo" : "";
        auto result = mpi_word_count(text);
        if (rank != 0) ASSERT_EQ((int)result.size(), 0);
    });

    // Test 7: many unique words
    RUN_TEST("many unique words no repeats", [&]() {
        std::string text = "";
        if (rank == 0) {
            for (int i = 0; i < 20; i++) {
                if (i > 0) text += ' ';
                text += "word" + std::to_string(i);
            }
        }
        auto result = mpi_word_count(text);
        if (rank == 0) {
            ASSERT_EQ((int)result.size(), 20);
            for (auto& [w, c] : result) ASSERT_EQ(c, 1);
        }
    });

    // Test 8: total count equals word count
    RUN_TEST("total word count", [&]() {
        std::string text = (rank == 0) ? "the quick brown fox jumps over the lazy dog the" : "";
        auto result = mpi_word_count(text);
        if (rank == 0) {
            int total = 0;
            for (auto& [w, c] : result) total += c;
            ASSERT_EQ(total, 10);
        }
    });

    // Test 9: case sensitivity
    RUN_TEST("case sensitive counting", [&]() {
        std::string text = (rank == 0) ? "Hello hello HELLO" : "";
        auto result = mpi_word_count(text);
        if (rank == 0) {
            ASSERT_EQ((int)result.size(), 3);
        }
    });

    MPI_Finalize();
    if (rank == 0) print_results();
    return 0;
}
