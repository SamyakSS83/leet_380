# 065 — Hybrid Reduction Custom

**Difficulty:** Medium · **Topic:** Hybrid OMP+MPI

## Problem

Find the **global minimum value** and its location `(rank, local_index)` across a distributed array using:

1. **OMP parallel reduction** — each rank finds its local minimum and the index where it occurs using OpenMP.
2. **MPI custom reduction** — define a custom `MPI_Op` that compares `MinLoc` structs (by value, breaking ties by rank then local_index) to find the global minimum location.

## Struct Definition

```cpp
struct MinLoc {
    double value;
    int rank;
    int local_index;
};
```

## Function Signature

```cpp
MinLoc hybrid_global_min(const std::vector<double>& local_data);
```

**Parameters:**
- `local_data` — this rank's portion of the distributed array

**Returns:** `MinLoc` with global minimum value, owning rank, and local index **on rank 0**. On other ranks, return a default `MinLoc{0.0, 0, 0}`.

## Tie-breaking

If two ranks have the same minimum value, the one with the **lower rank** wins. If same rank (impossible in practice), lower `local_index` wins.

## Example

```
Rank 0: [3.0, 1.0, 4.0]   → local min = 1.0 at index 1
Rank 1: [1.5, 0.5, 2.0]   → local min = 0.5 at index 1
Rank 2: [2.2, 3.3, 0.5]   → local min = 0.5 at index 2
Rank 3: [9.0, 7.0, 8.0]   → local min = 7.0 at index 1

Global min = 0.5, owned by rank 1 (lower rank wins tie)
Result: MinLoc{0.5, 1, 1}
```

## Constraints

- Each rank's array is non-empty
- Values are doubles (may be negative)
- All ranks call the function; only rank 0 returns meaningful result
