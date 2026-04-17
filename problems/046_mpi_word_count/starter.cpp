#include <mpi.h>
#include <vector>
#include <string>
#include <utility>

// text: full text on rank 0, "" on others.
// Returns sorted (word, count) pairs on rank 0, empty on others.
std::vector<std::pair<std::string, int>> mpi_word_count(const std::string& text) {
    // TODO: implement
    return {};
}
