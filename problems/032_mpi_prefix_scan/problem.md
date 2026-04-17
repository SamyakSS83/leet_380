# 032 — MPI Prefix Scan (Exclusive)

**Difficulty:** Medium · **Topic:** MPI, MPI_Exscan

## Problem

Each rank holds a single double value. Compute the exclusive prefix sum: rank `r` gets the sum of values from ranks 0 to r-1 (rank 0 gets 0).

**Implement:**
```cpp
double mpi_exclusive_prefix(double local_val);
```

Returns the exclusive prefix sum for this rank.

## Hints

<details><summary>Hint 1</summary>Use `MPI_Exscan` with `MPI_SUM`. Rank 0 result is undefined — set it to 0.</details>
