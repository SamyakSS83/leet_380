# 064 — Hybrid Floyd-Warshall

**Difficulty:** Hard · **Topic:** Hybrid OMP+MPI

## Problem

Compute All-Pairs Shortest Paths (APSP) using the Floyd-Warshall algorithm with a hybrid parallel approach:

- The `n×n` distance matrix is distributed **row-wise** across MPI ranks (each rank holds `n/num_ranks` rows).
- For each pivot `k` (0 to n-1):
  1. The rank owning row `k` broadcasts that row to all other ranks.
  2. Each rank updates its local rows using OpenMP to parallelize the inner loop.
- After all pivots, rank 0 gathers all rows and returns the full `n×n` result.

## Function Signature

```cpp
std::vector<std::vector<int>> hybrid_floyd_warshall(
    std::vector<std::vector<int>> local_dist, int n);
```

**Parameters:**
- `local_dist` — this rank's rows of the distance matrix; each row has `n` elements
- `n` — total number of vertices (matrix is `n×n`)
- Use `INT_MAX/2` (= 1073741823) to represent infinity (no edge)

**Returns:** Full `n×n` distance matrix on rank 0; empty vector on other ranks.

## Example

```
n=4, distances (INF = 1e9):
  0  1 INF INF
  1  0  1  INF
INF  1  0   1
INF INF  1   0

After APSP:
  0 1 2 3
  1 0 1 2
  2 1 0 1
  3 2 1 0
```

## Constraints

- n is a multiple of num_ranks
- 1 ≤ n ≤ 512
- Distances are non-negative integers
- Use `INT_MAX/2` for no-edge (to avoid overflow when adding)
- Self-distances are 0
