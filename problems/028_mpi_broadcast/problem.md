# 028 — MPI Broadcast (Binomial Tree)

**Difficulty:** Medium · **Topic:** MPI, broadcast, binomial tree

## Problem

Implement broadcast from rank 0 to all ranks using only `MPI_Send` and `MPI_Recv` in a binomial tree pattern. Do not use `MPI_Bcast`.

**Implement:**
```cpp
void my_bcast(std::vector<double>& buf, int root);
```

All ranks call this. After the call, all ranks have the root's data in `buf`.

## Hints

<details><summary>Hint 1</summary>In a binomial tree with P ranks: rank r receives from rank r-power, sends to r+power for increasing powers of 2.</details>
<details><summary>Hint 2</summary>Relative rank = (rank - root + size) % size to handle arbitrary root.</details>
