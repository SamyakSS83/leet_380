# 008 — Parallel Min and Max with Index

**Difficulty:** Easy · **Topic:** OpenMP, parallel reduction

## Problem

Given an array of doubles, find simultaneously:
- The minimum value and its first index
- The maximum value and its first index

**Implement:**
```cpp
struct MinMaxResult { double min_val; int min_idx; double max_val; int max_idx; };
MinMaxResult parallel_min_max_index(const std::vector<double>& v);
```

**Constraints:**
- 1 ≤ n ≤ 10^7
- If multiple elements share the same min/max value, return the smallest index

## Examples

```
Input:  [3.0, 1.0, 4.0, 1.5, 9.0, 2.0]
Output: min_val=1.0, min_idx=1, max_val=9.0, max_idx=4
```

## Hints

<details><summary>Hint 1</summary>Use `#pragma omp parallel` with per-thread local min/max, then combine in a critical section.</details>
<details><summary>Hint 2</summary>Track both value and index together.</details>
