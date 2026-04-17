# 048 — MPI One-Sided RMA Global Max

**Difficulty:** Hard · **Topic:** MPI, one-sided communication, MPI_Win, MPI_Accumulate, RMA

## Problem

Use MPI one-sided communication (RMA — Remote Memory Access) to compute the global maximum across all ranks without using `MPI_Reduce`. Each rank has a local double value. Rank 0 should end up with the global maximum.

**Implement:**
```cpp
double rma_global_max(double local_value);
// Returns the global maximum on rank 0, local_value on all other ranks.
```

Use `MPI_Win_create`, `MPI_Win_fence`, and `MPI_Accumulate` with `MPI_MAX` (or `MPI_Put`/`MPI_Get` with explicit logic) to implement this without `MPI_Reduce` or `MPI_Allreduce`.

## Hints

<details><summary>Hint 1</summary>Create a window on rank 0's memory. All ranks call MPI_Accumulate with MPI_MAX to accumulate into rank 0's window location. Use MPI_Win_fence to synchronize epochs.</details>
<details><summary>Hint 2</summary>MPI_Win_fence(0, win) acts as a barrier and marks epoch boundaries. Call it before and after the accumulate phase.</details>
