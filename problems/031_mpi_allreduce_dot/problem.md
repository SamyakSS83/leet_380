# 031 — MPI AllReduce Dot Product

**Difficulty:** Easy · **Topic:** MPI, MPI_Allreduce

## Problem

Compute a distributed dot product. The global vectors `a` and `b` of length `n` are split evenly across ranks. Each rank holds a chunk. Use `MPI_Allreduce` so all ranks get the result.

**Implement:**
```cpp
double mpi_allreduce_dot(const std::vector<double>& local_a,
                          const std::vector<double>& local_b);
```

Returns global dot product on all ranks.

## Hints

<details><summary>Hint 1</summary>Compute local partial dot product, then `MPI_Allreduce` with `MPI_SUM`.</details>
