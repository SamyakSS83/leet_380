#include <vector>
#include <omp.h>
#include <limits>

struct MinMaxResult {
    double min_val;
    int    min_idx;
    double max_val;
    int    max_idx;
};

MinMaxResult parallel_min_max_index(const std::vector<double>& v) {
    int n = (int)v.size();
    double gmin = std::numeric_limits<double>::max();
    double gmax = std::numeric_limits<double>::lowest();
    int gmin_i = 0, gmax_i = 0;

    #pragma omp parallel
    {
        double lmin = std::numeric_limits<double>::max();
        double lmax = std::numeric_limits<double>::lowest();
        int lmin_i = 0, lmax_i = 0;
        #pragma omp for schedule(static) nowait
        for (int i = 0; i < n; i++) {
            if (v[i] < lmin || (v[i] == lmin && i < lmin_i)) { lmin = v[i]; lmin_i = i; }
            if (v[i] > lmax || (v[i] == lmax && i < lmax_i)) { lmax = v[i]; lmax_i = i; }
        }
        #pragma omp critical
        {
            if (lmin < gmin || (lmin == gmin && lmin_i < gmin_i)) { gmin = lmin; gmin_i = lmin_i; }
            if (lmax > gmax || (lmax == gmax && lmax_i < gmax_i)) { gmax = lmax; gmax_i = lmax_i; }
        }
    }
    return {gmin, gmin_i, gmax, gmax_i};
}
