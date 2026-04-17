#include <vector>
#include <algorithm>
#include <omp.h>

static void merge_sort_helper(std::vector<int>& v, int lo, int hi, int cutoff) {
    if (hi - lo <= cutoff) {
        std::sort(v.begin() + lo, v.begin() + hi);
        return;
    }
    int mid = (lo + hi) / 2;
    #pragma omp task shared(v) firstprivate(lo, mid, cutoff)
    merge_sort_helper(v, lo, mid, cutoff);
    #pragma omp task shared(v) firstprivate(mid, hi, cutoff)
    merge_sort_helper(v, mid, hi, cutoff);
    #pragma omp taskwait
    std::inplace_merge(v.begin() + lo, v.begin() + mid, v.begin() + hi);
}

void parallel_merge_sort(std::vector<int>& v, int cutoff) {
    if (v.empty()) return;
    #pragma omp parallel
    {
        #pragma omp single
        merge_sort_helper(v, 0, (int)v.size(), cutoff);
    }
}
