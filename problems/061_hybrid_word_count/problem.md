# 061 — Hybrid Word Count

**Difficulty:** Medium · **Topic:** Hybrid OMP+MPI

## Problem

Count word frequencies in a text string using a hybrid parallel approach:

1. Rank 0 receives the full text and scatters chunks to all ranks.
2. Each rank splits its chunk into words and uses OpenMP with thread-local `unordered_map` to count frequencies.
3. Thread-local maps are merged into a rank-local map.
4. Ranks communicate to produce a global word count (e.g., via serialization and `MPI_Reduce`/`MPI_Gather`).
5. Rank 0 returns a sorted vector of `(word, count)` pairs.

## Function Signature

```cpp
std::vector<std::pair<std::string,int>> hybrid_word_count(const std::string& text);
```

**Parameters:**
- `text` — full text on rank 0; empty string `""` on other ranks

**Returns:** Sorted `(word, count)` pairs on rank 0; empty vector on other ranks.

Sorting is lexicographic by word.

## Example

```
text = "the cat sat on the mat the cat"
result = [("cat",2), ("mat",1), ("on",1), ("sat",1), ("the",3)]
```

## Constraints

- Words are separated by spaces (one or more)
- All lowercase alphabetic words (no punctuation)
- text length ≤ 1,000,000 characters
- All ranks must participate in MPI communication
