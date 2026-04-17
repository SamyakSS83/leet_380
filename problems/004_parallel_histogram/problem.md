# 004 — Parallel Histogram

**Difficulty:** Easy · **Topic:** OpenMP, atomic updates

## Problem

Given an array of integers and a number of bins `B`, compute the histogram. Each element `v[i]` falls in bin `v[i] % B`. Use atomic updates or reduction to avoid races.

**Implement:**
```cpp
std::vector<int> parallel_histogram(const std::vector<int>& v, int B);
```

**Constraints:**
- 0 ≤ n ≤ 10^7
- 1 ≤ B ≤ 1000
- All values v[i] ≥ 0

## Examples

```
Input:  v=[0,1,2,3,4,5], B=3
Output: [2, 2, 2]   (bins: {0,3}, {1,4}, {2,5})
```

## Hints

<details><summary>Hint 1</summary>Use `#pragma omp atomic` before `hist[bin]++`.</details>
<details><summary>Hint 2</summary>Alternatively, each thread maintains a local histogram and then combine with a critical section.</details>
