// tests.cpp for problem 019 — Parallel Word Count
#include "harness.h"
#include <string>
#include <utility>
#include <map>

std::vector<std::pair<std::string,int>> parallel_word_count(
    const std::vector<std::string>& words);

void test_empty() {
    auto r = parallel_word_count({});
    ASSERT_TRUE(r.empty(), "expected empty");
}

void test_single() {
    auto r = parallel_word_count({"hello"});
    ASSERT_EQ((int)r.size(), 1);
    ASSERT_TRUE(r[0].first == "hello", "word mismatch");
    ASSERT_EQ(r[0].second, 1);
}

void test_duplicates() {
    auto r = parallel_word_count({"a","b","a","c","b","a"});
    std::map<std::string,int> m(r.begin(), r.end());
    ASSERT_EQ(m["a"], 3); ASSERT_EQ(m["b"], 2); ASSERT_EQ(m["c"], 1);
}

void test_sorted_output() {
    auto r = parallel_word_count({"zoo","apple","mango","apple"});
    ASSERT_TRUE(r[0].first == "apple", "not sorted");
    ASSERT_TRUE(r[1].first == "mango", "not sorted");
    ASSERT_TRUE(r[2].first == "zoo", "not sorted");
}

void test_all_same() {
    std::vector<std::string> words(100, "word");
    auto r = parallel_word_count(words);
    ASSERT_EQ((int)r.size(), 1);
    ASSERT_EQ(r[0].second, 100);
}

void test_large() {
    std::vector<std::string> words;
    std::string vocab[] = {"alpha","beta","gamma","delta","epsilon"};
    for (int i = 0; i < 10000; i++) words.push_back(vocab[i%5]);
    auto r = parallel_word_count(words);
    ASSERT_EQ((int)r.size(), 5);
    for (auto& [w,c] : r) ASSERT_EQ(c, 2000);
}

void test_unique_words() {
    std::vector<std::string> words;
    for (int i = 0; i < 26; i++) words.push_back(std::string(1, 'a'+i));
    auto r = parallel_word_count(words);
    ASSERT_EQ((int)r.size(), 26);
    for (auto& [w,c] : r) ASSERT_EQ(c, 1);
}

int main() {
    RUN_TEST("empty", test_empty);
    RUN_TEST("single word", test_single);
    RUN_TEST("duplicates", test_duplicates);
    RUN_TEST("sorted output", test_sorted_output);
    RUN_TEST("all same", test_all_same);
    RUN_TEST("large balanced", test_large);
    RUN_TEST("all unique", test_unique_words);
    print_results();
    return 0;
}
