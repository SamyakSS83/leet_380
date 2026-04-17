# 040 — MPI Monte Carlo Pi

**Difficulty:** Easy · **Topic:** MPI, Monte Carlo, MPI_Reduce

## Problem

Estimate π using Monte Carlo. Each rank generates `n_local` samples with its own seed. Use `MPI_Reduce` to sum total hits and estimate π at rank 0.

**Implement:**
```cpp
double mpi_monte_carlo_pi(long long n_local, unsigned base_seed);
```

Returns π estimate on rank 0, 0.0 on other ranks.

## Hints

<details><summary>Hint 1</summary>Each rank uses seed `base_seed + rank`. Use `rand_r` for thread-safe random numbers.</details>
