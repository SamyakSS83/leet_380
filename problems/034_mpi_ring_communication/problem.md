# 034 — MPI Ring Communication

**Difficulty:** Easy · **Topic:** MPI, ring, point-to-point

## Problem

Pass a token around a ring of ranks. Rank 0 starts with token=0. Each rank adds its rank value to the token and passes it to the next. After a full round, rank 0 holds the sum of all ranks.

**Implement:**
```cpp
int mpi_ring_sum();
```

Returns the final token value on rank 0 (0 on others). Final value = 0+1+2+...+(size-1).

## Hints

<details><summary>Hint 1</summary>Rank r receives from (r-1+size)%size, adds r, sends to (r+1)%size. Use tag to avoid deadlocks.</details>
