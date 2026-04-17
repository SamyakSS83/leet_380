# 053 — MPI Broadcast Tree

**Difficulty:** Hard · **Topic:** MPI, Binomial Tree, Point-to-Point

## Problem

Implement a broadcast using **only** `MPI_Send` and `MPI_Recv` — no `MPI_Bcast` or collective operations. Use the **binomial tree algorithm**: at step `k`, rank `r` that has the data sends to rank `r + 2^k` (if it exists and hasn't received yet).

**Implement:**
```cpp
void tree_broadcast(std::vector<int>& data, int root);
```

After the call, all ranks hold the same `data` vector that `root` had before the call.

## Constraints
- Any rank may be the root (0 ≤ root < size).
- Data vector is non-empty.
- Use only `MPI_Send` / `MPI_Recv`.

## Hints

<details><summary>Hint 1</summary>Translate ranks to a "virtual rank" relative to root: `vrank = (rank - root + size) % size`. Then the binomial tree operates on virtual ranks, translate back to send.</details>
<details><summary>Hint 2</summary>At step k (for k=0,1,...,log2(size)): if vrank has bit k clear and vrank < 2^k, it sends to vrank + 2^k. If vrank == 2^k, it receives from vrank - 2^k (i.e., its parent).</details>
