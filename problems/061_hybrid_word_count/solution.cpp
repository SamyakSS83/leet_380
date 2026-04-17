#include <mpi.h>
#include <omp.h>
#include <vector>
#include <string>
#include <utility>
#include <unordered_map>
#include <map>
#include <sstream>
#include <algorithm>

std::vector<std::pair<std::string,int>> hybrid_word_count(const std::string& text) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Rank 0 splits text into words, scatters word-chunks (never cuts mid-word)
    std::vector<int> word_counts(size, 0), word_displs(size, 0);
    std::vector<std::string> all_words_flat;  // only on rank 0
    std::string local_text;

    if (rank == 0) {
        std::istringstream iss(text);
        std::string w;
        while (iss >> w) all_words_flat.push_back(w);
        int total = (int)all_words_flat.size();
        for (int r = 0; r < size; r++) {
            word_displs[r] = (int)(long long)r * total / size;
            word_counts[r] = (int)(long long)(r+1) * total / size - word_displs[r];
        }
    }
    MPI_Bcast(word_counts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(word_displs.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

    // Build per-rank text chunk on rank 0, scatter by char length
    std::vector<int> char_counts(size, 0), char_displs(size, 0);
    std::string packed;
    if (rank == 0) {
        std::vector<std::string> chunks(size);
        for (int r = 0; r < size; r++) {
            for (int i = word_displs[r]; i < word_displs[r] + word_counts[r]; i++) {
                if (!chunks[r].empty()) chunks[r] += ' ';
                chunks[r] += all_words_flat[i];
            }
        }
        int off = 0;
        for (int r = 0; r < size; r++) {
            char_displs[r] = off;
            char_counts[r] = (int)chunks[r].size();
            packed += chunks[r];
            off += char_counts[r];
        }
    }
    MPI_Bcast(char_counts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
    int local_len = char_counts[rank];
    local_text.resize(local_len);
    MPI_Scatterv(rank == 0 ? packed.data() : nullptr,
                 char_counts.data(), char_displs.data(), MPI_CHAR,
                 local_text.data(), local_len, MPI_CHAR, 0, MPI_COMM_WORLD);

    // Each rank counts words using OMP thread-local maps
    int nthreads = omp_get_max_threads();
    std::vector<std::unordered_map<std::string,int>> thread_maps(nthreads);

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int tsize = omp_get_num_threads();
        // split local_text by words across threads
        std::istringstream ss_all(local_text);
        std::vector<std::string> local_words;
        std::string ww;
        while (ss_all >> ww) local_words.push_back(ww);
        int wstart = (int)(long long)tid * local_words.size() / tsize;
        int wend   = (int)(long long)(tid+1) * local_words.size() / tsize;
        for (int i = wstart; i < wend; i++)
            thread_maps[tid][local_words[i]]++;
    }

    // Merge thread-local maps
    std::unordered_map<std::string,int> local_map;
    for (auto& m : thread_maps)
        for (auto& [w, c] : m)
            local_map[w] += c;

    // Serialize local_map to a string: "word:count\n..."
    std::string serial;
    for (auto& [w, c] : local_map) {
        serial += w + ":" + std::to_string(c) + "\n";
    }

    // Gather all serialized maps to rank 0
    int slen = (int)serial.size();
    std::vector<int> all_lens(size);
    MPI_Gather(&slen, 1, MPI_INT, all_lens.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> all_displs(size, 0);
    int total_serial = 0;
    std::string gathered;
    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            all_displs[i] = total_serial;
            total_serial += all_lens[i];
        }
        gathered.resize(total_serial);
    }

    MPI_Gatherv(serial.data(), slen, MPI_CHAR,
                rank == 0 ? gathered.data() : nullptr,
                all_lens.data(), all_displs.data(), MPI_CHAR,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Parse and merge
        std::map<std::string,int> global_map;
        std::istringstream ss(gathered);
        std::string line;
        while (std::getline(ss, line)) {
            if (line.empty()) continue;
            auto pos = line.rfind(':');
            if (pos == std::string::npos) continue;
            std::string w = line.substr(0, pos);
            int c = std::stoi(line.substr(pos+1));
            global_map[w] += c;
        }
        std::vector<std::pair<std::string,int>> result(global_map.begin(), global_map.end());
        return result;
    }
    return {};
}
