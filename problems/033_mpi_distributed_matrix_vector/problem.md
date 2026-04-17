# 033 — MPI Distributed Matrix-Vector Multiply

**Difficulty:** Medium · **Topic:** MPI, row-distributed matvec

## Problem

Perform `y = A * x` where `A` is distributed row-wise across ranks. Rank 0 holds the full `x` vector (broadcast to all). Each rank computes its rows of `y`, then gather to rank 0.

**Implement:**
```cpp
std::vector<double> mpi_matvec(
    const std::vector<double>& local_rows_flat,
    const std::vector<double>& x_full,
    int local_nrows, int ncols);
```

Returns the full `y` on rank 0 (empty on others). `local_rows_flat` is row-major.

## Hints

<details><summary>Hint 1</summary>Broadcast `x_full` to all ranks, compute local rows of y, then `MPI_Gather`.</details>
