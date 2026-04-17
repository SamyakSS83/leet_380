# 055 — MPI Distributed Sum Verify

**Difficulty:** Medium · **Topic:** MPI, MPI_Scatter, MPI_Allreduce, Verification

## Problem

Scatter an array from rank 0 to all ranks. Each rank computes its partial sum. Verify that the total of all partial sums equals the reference sum computed on rank 0. Return `true` on all ranks if consistent, `false` otherwise.

**Implement:**
```cpp
bool verify_distributed_sum(const std::vector<double>& full_array);
```

- `full_array` is provided on rank 0 (size must be divisible by number of MPI ranks), empty on others.
- Returns `true` on **all** ranks if the distributed computation is consistent.
- Use `MPI_Scatter` to distribute chunks, then `MPI_Allreduce` to sum partial results and broadcast a consistency flag.

## Constraints
- `full_array.size() % size == 0` (guaranteed divisible by nprocs).
- Elements may be any finite double.

## Hints

<details><summary>Hint 1</summary>Scatter chunks of size `n/size` to each rank. Compute local partial sum. Use MPI_Allreduce with MPI_SUM to get the global sum on all ranks.</details>
<details><summary>Hint 2</summary>Rank 0 already knows the reference sum (sum of full_array). Broadcast it, then compare with the allreduced sum on all ranks.</details>
