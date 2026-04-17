# 030 — MPI Reduce Sum

**Difficulty:** Easy · **Topic:** MPI, MPI_Reduce

## Problem

Each rank holds a partial array of doubles. Use `MPI_Reduce` to compute the global sum at rank 0.

**Implement:**
```cpp
double mpi_distributed_sum(const std::vector<double>& local_data);
```

Returns the global sum on rank 0; 0.0 on other ranks.

## Hints

<details><summary>Hint 1</summary>Sum local_data first, then `MPI_Reduce` with `MPI_SUM`.</details>
