# 020 — Max Subarray Sum (Parallel Divide-and-Conquer)

**Difficulty:** Hard · **Topic:** OpenMP, divide and conquer, reduction

## Problem

Find the maximum subarray sum (Kadane's problem) using parallel divide-and-conquer with OMP tasks. An empty subarray has sum 0.

**Implement:**
```cpp
double max_subarray_sum(const std::vector<double>& v);
```

**Constraints:**
- 0 ≤ n ≤ 10^6
- All values may be negative (return 0 for empty subarray convention)

## Examples

```
Input:  [-2, 1, -3, 4, -1, 2, 1, -5, 4]
Output: 6.0   (subarray [4,-1,2,1])
```

## Hints

<details><summary>Hint 1</summary>For divide-and-conquer: each segment stores (total, max_prefix, max_suffix, max_sub). These combine in O(1).</details>
<details><summary>Hint 2</summary>Use OMP tasks for the two halves, taskwait, then combine.</details>
