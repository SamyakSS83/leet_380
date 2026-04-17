# 005 — Monte Carlo Pi (OpenMP)

**Difficulty:** Easy · **Topic:** OpenMP, reduction, random

## Problem

Estimate π using Monte Carlo: generate `n` random points (x,y) in [0,1)x[0,1), count how many fall inside the unit circle (x²+y²<1), and return `4.0 * inside / n`.

Use per-thread seed arrays to ensure thread safety. Use a fixed global seed for reproducibility.

**Implement:**
```cpp
double monte_carlo_pi(long long n, unsigned seed);
```

**Constraints:**
- 1 ≤ n ≤ 10^8
- Return estimate of π (should be within 0.01 of 3.14159 for n ≥ 10^6)

## Examples

```
Input:  n=1000000, seed=42
Output: ~3.14159  (within 0.02)
```

## Hints

<details><summary>Hint 1</summary>Use per-thread `unsigned` seeds in an array indexed by `omp_get_thread_num()` to avoid race conditions on the RNG state.</details>
<details><summary>Hint 2</summary>Use `reduction(+:inside)` to count hits.</details>
