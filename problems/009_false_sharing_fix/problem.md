# 009 — False Sharing Fix

**Difficulty:** Medium · **Topic:** OpenMP, cache, padding

## Problem

Demonstrate and fix false sharing. Each thread accumulates a partial sum into its own array slot. Without padding, nearby slots share a cache line causing false sharing. With padding (64-byte aligned slots), performance improves.

**Implement two functions:**
```cpp
double sum_with_false_sharing(const std::vector<double>& v, int nthreads);
double sum_with_padding(const std::vector<double>& v, int nthreads);
```

Both must return the correct sum. The padded version should avoid false sharing by ensuring each thread's slot is on its own cache line.

**Constraints:**
- 1 ≤ n ≤ 10^7
- Both functions must return the same correct answer

## Hints

<details><summary>Hint 1</summary>False sharing: `double partial[T]` — all T slots may fit in a few cache lines.</details>
<details><summary>Hint 2</summary>Fix: use a struct with padding `struct alignas(64) Padded { double val; };`</details>
