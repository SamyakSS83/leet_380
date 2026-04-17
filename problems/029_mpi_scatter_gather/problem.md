# 029 — MPI Scatter/Gather: Square Elements

**Difficulty:** Easy · **Topic:** MPI, MPI_Scatter, MPI_Gather

## Problem

Rank 0 has an array of `n` doubles (n divisible by number of ranks). Scatter equal chunks to each rank, each rank squares its chunk, gather back to rank 0.

**Implement:**
```cpp
std::vector<double> scatter_square_gather(std::vector<double> data);
```

`data` is only meaningful on rank 0 (size n). Returns squared array on rank 0, empty on others.

## Hints

<details><summary>Hint 1</summary>Use `MPI_Scatter` to send chunks, each rank squares locally, `MPI_Gather` to collect.</details>
