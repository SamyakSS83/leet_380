# 060 — Hybrid Monte Carlo Pi

**Difficulty:** Easy · **Topic:** Hybrid OMP+MPI

## Problem

Estimate π using Monte Carlo sampling with a hybrid parallel approach:

1. Each MPI rank spawns multiple OpenMP threads.
2. Each thread generates `samples_per_thread` random (x, y) points in [0,1)².
3. Count points inside the unit circle (x²+y² < 1).
4. Use `MPI_Reduce` to sum all counts across ranks.
5. Rank 0 computes and returns the estimate: `4 * total_inside / total_samples`.

Total samples = `samples_per_thread × omp_get_max_threads() × num_ranks`.

## Function Signature

```cpp
double hybrid_monte_carlo_pi(long long samples_per_thread);
```

**Parameters:**
- `samples_per_thread` — number of samples each OpenMP thread generates

**Returns:** π estimate on rank 0; 0.0 on other ranks.

## Example

With 4 MPI ranks × 2 OMP threads × 1,000,000 samples/thread = 8M total samples,
the estimate should be within 0.01 of π ≈ 3.14159.

## Constraints

- samples_per_thread ≥ 1
- Use `rand_r` or similar thread-safe RNG seeded by `rank * max_threads + thread_id`
- All ranks must call MPI_Reduce
