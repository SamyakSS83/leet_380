# 039 — MPI Distributed Histogram

**Difficulty:** Easy · **Topic:** MPI, allreduce, histogram

## Problem

Each rank holds a local array of integers. Compute the global histogram with `B` bins (bin = value % B) using `MPI_Allreduce`. All ranks get the result.

**Implement:**
```cpp
std::vector<int> mpi_histogram(const std::vector<int>& local_data, int B);
```

Returns histogram of size B on all ranks.

## Hints

<details><summary>Hint 1</summary>Build local histogram, `MPI_Allreduce` with `MPI_SUM` over all B bins.</details>
