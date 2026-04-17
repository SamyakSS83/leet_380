#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <omp.h>

std::vector<std::pair<std::string,int>> parallel_word_count(
    const std::vector<std::string>& words) {
    int n = (int)words.size();
    int nthreads = omp_get_max_threads();
    std::vector<std::unordered_map<std::string,int>> local(nthreads);

    #pragma omp parallel num_threads(nthreads)
    {
        int tid = omp_get_thread_num();
        auto& lmap = local[tid];
        #pragma omp for schedule(static) nowait
        for (int i = 0; i < n; i++) lmap[words[i]]++;
    }

    std::map<std::string,int> merged;
    for (auto& lmap : local)
        for (auto& [w, c] : lmap) merged[w] += c;

    return std::vector<std::pair<std::string,int>>(merged.begin(), merged.end());
}
