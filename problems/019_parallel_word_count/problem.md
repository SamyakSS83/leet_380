# 019 — Parallel Word Count

**Difficulty:** Medium · **Topic:** OpenMP, reduction, string processing

## Problem

Given a vector of words (lowercase, no punctuation), count the frequency of each word and return a sorted vector of `(word, count)` pairs sorted by word alphabetically.

**Implement:**
```cpp
std::vector<std::pair<std::string,int>> parallel_word_count(
    const std::vector<std::string>& words);
```

**Constraints:**
- 0 ≤ n ≤ 10^6 words
- Words contain only lowercase letters a-z

## Hints

<details><summary>Hint 1</summary>Build per-thread local `unordered_map`, then merge in a critical section or after the parallel region.</details>
<details><summary>Hint 2</summary>Sort the final map by key before returning.</details>
