# 026 — MPI Hello World

**Difficulty:** Easy · **Topic:** MPI basics, ranks, communicators

## Problem

Each MPI rank prints "Hello from rank R of S" (collecting messages to rank 0). Rank 0 prints all messages sorted by rank.

**Implement:**
```cpp
// Fills msgs on rank 0 with "Hello from rank R of S" for each R, sorted.
std::vector<std::string> mpi_hello(int argc, char** argv);
```

**Constraints:**
- Works for any number of ranks ≥ 1

## Hints

<details><summary>Hint 1</summary>Each rank creates its message. Use MPI_Gather with a fixed buffer size to collect all strings at rank 0.</details>
