# 046 — MPI Word Count

**Difficulty:** Medium · **Topic:** MPI, MPI_Scatter, string processing, collective communication

## Problem

Count word frequencies in a text string distributed across ranks. Rank 0 receives the full text; all ranks collaborate to count words and produce global frequency counts on rank 0.

**Implement:**
```cpp
std::vector<std::pair<std::string, int>> mpi_word_count(const std::string& text);
// `text` is the full text on rank 0; other ranks pass ""
// Returns sorted (lexicographic) vector of (word, count) pairs on rank 0, empty on others.
```

Words are separated by whitespace. Case-sensitive.

## Hints

<details><summary>Hint 1</summary>Rank 0 splits text into size roughly-equal chunks and scatters them. Each rank counts its local words. Gather all local maps to rank 0 and merge.</details>
<details><summary>Hint 2</summary>Use MPI_Gather with variable lengths (MPI_Gatherv) to collect serialized word-count data, or just gather all partial counts to rank 0 for merging.</details>
