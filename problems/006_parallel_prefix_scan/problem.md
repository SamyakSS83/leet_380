# 006 — Parallel Prefix Scan

**Difficulty:** Medium · **Topic:** OpenMP, parallel scan

## Problem

Compute the inclusive prefix sum (scan) of an array of doubles in parallel.

Output `out[i] = v[0] + v[1] + ... + v[i]`.

**Implement:**
```cpp
std::vector<double> parallel_prefix_scan(const std::vector<double>& v);
```

**Constraints:**
- 0 ≤ n ≤ 10^7

## Examples

```
Input:  [1, 2, 3, 4, 5]
Output: [1, 3, 6, 10, 15]
```

## Hints

<details><summary>Hint 1</summary>Partition into chunks, compute local prefix sums in parallel, then adjust each chunk by the sum of all previous chunks.</details>
<details><summary>Hint 2</summary>Phase 1: each thread computes its chunk's total. Phase 2: compute offsets for each chunk. Phase 3: each thread adds offset to its local scan.</details>
