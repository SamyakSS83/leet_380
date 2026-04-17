# 056 — Hybrid Hello World

**Difficulty:** Easy · **Topic:** MPI + OpenMP, Hybrid Parallelism

## Problem

Each OpenMP thread on each MPI rank prints a hello message. Collect all messages and return them **sorted** on rank 0.

Each message has the form:
```
Hello from rank R thread T of S ranks N threads
```
where R = MPI rank, T = OpenMP thread index, S = total MPI ranks, N = num_threads.

**Implement:**
```cpp
std::vector<std::string> hybrid_hello(int num_threads);
```

- Returns a sorted vector of all messages on rank 0.
- Returns empty on other ranks.

## Constraints
- num_threads ≥ 1.
- Messages must exactly match the format above.
- Use `omp_set_num_threads` and `omp_get_thread_num`.

## Hints

<details><summary>Hint 1</summary>Each rank generates its local messages inside an OMP parallel region, gathers them on rank 0 via MPI, then rank 0 sorts.</details>
<details><summary>Hint 2</summary>To gather variable-length strings, serialize to a flat char buffer or use MPI_Gatherv on a fixed-size string array.</details>
