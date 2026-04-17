#include <vector>
#include <algorithm>
#include <omp.h>

static void qs_helper(std::vector<int>& v, int lo, int hi, int cutoff) {
    if (hi - lo <= 1) return;
    if (hi - lo <= cutoff) {
        std::sort(v.begin() + lo, v.begin() + hi);
        return;
    }
    // three-way (Dutch national flag) partition to handle duplicates
    int mid = lo + (hi - lo) / 2;
    if (v[lo] > v[mid]) std::swap(v[lo], v[mid]);
    if (v[lo] > v[hi-1]) std::swap(v[lo], v[hi-1]);
    if (v[mid] > v[hi-1]) std::swap(v[mid], v[hi-1]);
    int pivot = v[mid];
    // partition into <pivot | ==pivot | >pivot
    int lt = lo, eq = lo, gt = hi;
    while (eq < gt) {
        if (v[eq] < pivot)       std::swap(v[lt++], v[eq++]);
        else if (v[eq] == pivot) ++eq;
        else                     std::swap(v[eq], v[--gt]);
    }
    // lt..eq is the "==" region; guaranteed progress
    #pragma omp task shared(v) firstprivate(lo, lt, cutoff)
    qs_helper(v, lo, lt, cutoff);
    #pragma omp task shared(v) firstprivate(eq, hi, cutoff)
    qs_helper(v, eq, hi, cutoff);
    #pragma omp taskwait
}

void parallel_quicksort(std::vector<int>& v, int cutoff) {
    if (v.size() <= 1) return;
    #pragma omp parallel
    {
        #pragma omp single
        qs_helper(v, 0, (int)v.size(), cutoff);
    }
}
