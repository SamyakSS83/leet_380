#include <mpi.h>
#include <omp.h>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>

// Hybrid word count: rank 0 scatters text, each rank uses OMP threads with
// thread-local maps, results gathered and merged on rank 0.
// Returns sorted (word, count) pairs on rank 0, empty on others.
std::vector<std::pair<std::string,int>> hybrid_word_count(const std::string& text) {
    // TODO: implement
    return {};
}
