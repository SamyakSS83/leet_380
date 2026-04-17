# 001 — Parallel Sum

**Difficulty:** Easy · **Topic:** OpenMP, parallel reduction

## Problem

Given an array of `n` doubles, compute the sum of all elements using OpenMP parallelism.

**Implement:**
```cpp
double parallel_sum(const std::vector<double>& v);
```

**Constraints:**
- 0 ≤ n ≤ 10^7
- Elements in [-10^6, 10^6]
- Use `#pragma omp parallel for reduction(+:sum)` or equivalent

## Examples

```
Input:  [1.0, 2.0, 3.0, 4.0, 5.0]
Output: 15.0

Input:  []
Output: 0.0
```

## Notes

The sequential sum is `O(n)`. A correct parallel implementation should produce
the same result (within floating-point tolerance) and scale with thread count.

## Hints

<details><summary>Hint 1</summary>Use a `reduction(+:sum)` clause to avoid data races on the accumulator.</details>
<details><summary>Hint 2</summary>Initialize `sum = 0.0` before the parallel region.</details>
