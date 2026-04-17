# 057 — Hybrid Parallel Sum

**Difficulty:** Easy · **Topic:** MPI + OpenMP, Hybrid Parallelism, Reduction

## Problem

Compute the global sum of an array using both MPI and OpenMP:
1. Rank 0 **scatters** equal chunks of the array to all ranks via `MPI_Scatter`.
2. Each rank computes its **local sum** using an OpenMP parallel reduction.
3. Use `MPI_Reduce` to sum all local sums on rank 0.

**Implement:**
```cpp
double hybrid_sum(const std::vector<double>& data);
```

- `data` is provided on rank 0, empty on others.
- `data.size()` is divisible by the number of MPI ranks.
- Returns the global sum on rank 0; other ranks return 0.0.

## Constraints
- Use `#pragma omp parallel for reduction(+:local_sum)` for the OMP part.
- Use `MPI_Scatter` + `MPI_Reduce` (or equivalent).

## Hints

<details><summary>Hint 1</summary>Broadcast the array size first so all ranks know their chunk size.</details>
<details><summary>Hint 2</summary>Each rank reduces its chunk with OMP, then MPI_Reduce sums across ranks.</details>
