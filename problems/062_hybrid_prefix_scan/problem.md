# 062 — Hybrid Prefix Scan

**Difficulty:** Hard · **Topic:** Hybrid OMP+MPI

## Problem

Compute a **global exclusive prefix sum** (scan) of a distributed array using a three-phase hybrid approach:

1. **OMP local scan** — each rank computes a local exclusive prefix sum of its chunk using OpenMP.
2. **MPI inter-rank offset** — use `MPI_Exscan` to compute the sum of all elements on ranks with lower rank IDs. This gives each rank its "offset" into the global prefix sum.
3. **OMP offset adjustment** — each rank adds the inter-rank offset to all its local results using OpenMP.

The result is the global exclusive prefix sum distributed across ranks.

## Function Signature

```cpp
std::vector<double> hybrid_prefix_scan(const std::vector<double>& local_data);
```

**Parameters:**
- `local_data` — this rank's portion of the global array

**Returns:** Each rank returns its portion of the global exclusive prefix sum.
(In tests, rank 0 gathers all portions for verification.)

## Example

Global array (4 ranks, 3 elements each):
```
Rank 0: [1, 2, 3]   → local excl prefix: [0, 1, 3]   → offset=0  → result: [0, 1, 3]
Rank 1: [4, 5, 6]   → local excl prefix: [0, 4, 9]   → offset=6  → result: [6, 10, 15]
Rank 2: [7, 8, 9]   → local excl prefix: [0, 7, 15]  → offset=21 → result: [21, 28, 36]
Rank 3: [10,11,12]  → local excl prefix: [0,10, 21]  → offset=45 → result: [45, 55, 66]
```

Global exclusive prefix sum: `[0,1,3,6,10,15,21,28,36,45,55,66]`

## Constraints

- Each rank holds the same number of elements
- Values are doubles
- All ranks participate in MPI calls
