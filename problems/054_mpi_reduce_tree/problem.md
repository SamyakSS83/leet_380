# 054 — MPI Reduce Tree

**Difficulty:** Hard · **Topic:** MPI, Binomial Tree, Point-to-Point, Reduction

## Problem

Implement a sum reduction using **only** `MPI_Send` and `MPI_Recv` — no `MPI_Reduce` or any collective. Use the **binomial tree algorithm**: at each step, non-root ranks send their partial sum up the tree.

**Implement:**
```cpp
double tree_reduce_sum(double local_value, int root);
```

- Returns the global sum on `root`.
- Return value on non-root ranks is unspecified.

## Constraints
- Any rank may be the root (0 ≤ root < size).
- Use only `MPI_Send` / `MPI_Recv`.

## Hints

<details><summary>Hint 1</summary>Use virtual ranks relative to root: `vrank = (rank - root + size) % size`.</details>
<details><summary>Hint 2</summary>At each step k, if bit k of vrank is set, send accumulated sum to parent (vrank ^ (1<<k)), then stop. Otherwise receive from child (vrank | (1<<k)) if that child exists, and add to local sum.</details>
