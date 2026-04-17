# 002 — Parallel Dot Product

**Difficulty:** Easy · **Topic:** OpenMP, parallel reduction

## Problem

Given two vectors `a` and `b` of length `n`, compute their dot product using OpenMP parallelism.

**Implement:**
```cpp
double parallel_dot_product(const std::vector<double>& a, const std::vector<double>& b);
```

**Constraints:**
- 0 ≤ n ≤ 10^7
- Both vectors have the same length
- Return 0.0 if vectors are empty

## Examples

```
Input:  a=[1,2,3], b=[4,5,6]
Output: 32.0   (1*4 + 2*5 + 3*6)

Input:  a=[], b=[]
Output: 0.0
```

## Hints

<details><summary>Hint 1</summary>Use `reduction(+:dot)` over `a[i]*b[i]`.</details>
<details><summary>Hint 2</summary>Ensure both arrays are accessed read-only inside the parallel region.</details>
