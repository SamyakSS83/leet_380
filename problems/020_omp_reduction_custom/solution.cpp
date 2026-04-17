#include <vector>
#include <algorithm>
#include <omp.h>

struct Seg {
    double total, max_pre, max_suf, max_sub;
};

static Seg combine(Seg L, Seg R) {
    return {
        L.total + R.total,
        std::max(L.max_pre, L.total + R.max_pre),
        std::max(R.max_suf, R.total + L.max_suf),
        std::max({L.max_sub, R.max_sub, L.max_suf + R.max_pre})
    };
}

static Seg solve(const std::vector<double>& v, int lo, int hi, int cutoff) {
    if (hi - lo == 1) {
        double x = v[lo];
        return {x, std::max(0.0, x), std::max(0.0, x), std::max(0.0, x)};
    }
    int mid = (lo + hi) / 2;
    Seg L, R;
    if (hi - lo > cutoff) {
        #pragma omp task shared(L) firstprivate(lo, mid, cutoff)
        L = solve(v, lo, mid, cutoff);
        #pragma omp task shared(R) firstprivate(mid, hi, cutoff)
        R = solve(v, mid, hi, cutoff);
        #pragma omp taskwait
    } else {
        L = solve(v, lo, mid, cutoff);
        R = solve(v, mid, hi, cutoff);
    }
    return combine(L, R);
}

double max_subarray_sum(const std::vector<double>& v) {
    if (v.empty()) return 0.0;
    Seg result;
    #pragma omp parallel
    {
        #pragma omp single
        result = solve(v, 0, (int)v.size(), 256);
    }
    return result.max_sub;
}
