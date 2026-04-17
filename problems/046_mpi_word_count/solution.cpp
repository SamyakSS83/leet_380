#include <mpi.h>
#include <vector>
#include <string>
#include <utility>
#include <map>
#include <sstream>
#include <algorithm>

std::vector<std::pair<std::string, int>> mpi_word_count(const std::string& text) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Rank 0 splits text into chunks and scatters
    std::vector<int> chunk_lens(size, 0);
    std::string full_text = text;

    if (rank == 0) {
        // Split into words, then reassemble chunks
        std::vector<std::string> words;
        std::istringstream iss(text);
        std::string w;
        while (iss >> w) words.push_back(w);

        // Divide words into size chunks
        int total_words = (int)words.size();
        std::vector<std::string> chunks(size);
        for (int r = 0; r < size; r++) {
            int start = (long long)r * total_words / size;
            int end   = (long long)(r+1) * total_words / size;
            for (int i = start; i < end; i++) {
                if (i > start) chunks[r] += ' ';
                chunks[r] += words[i];
            }
            chunk_lens[r] = (int)chunks[r].size();
        }

        // Send chunk lengths
        MPI_Scatter(chunk_lens.data(), 1, MPI_INT, MPI_IN_PLACE, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Send chunks with Scatterv
        std::string all_chunks;
        std::vector<int> displs(size, 0);
        for (int r = 0; r < size; r++) {
            displs[r] = (int)all_chunks.size();
            all_chunks += chunks[r];
        }
        int my_len = chunk_lens[0];
        std::string my_chunk(my_len, ' ');
        MPI_Scatterv(all_chunks.data(), chunk_lens.data(), displs.data(), MPI_CHAR,
                     my_chunk.data(), my_len, MPI_CHAR, 0, MPI_COMM_WORLD);
        full_text = my_chunk;
    } else {
        int my_len = 0;
        MPI_Scatter(nullptr, 1, MPI_INT, &my_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
        std::string my_chunk(my_len, ' ');
        MPI_Scatterv(nullptr, nullptr, nullptr, MPI_CHAR,
                     my_chunk.data(), my_len, MPI_CHAR, 0, MPI_COMM_WORLD);
        full_text = my_chunk;
    }

    // Count local words
    std::map<std::string, int> local_counts;
    std::istringstream iss2(full_text);
    std::string w;
    while (iss2 >> w) local_counts[w]++;

    // Serialize local counts: "word count word count ..."
    std::string serialized;
    for (auto& [word, cnt] : local_counts) {
        serialized += word + ' ' + std::to_string(cnt) + '\n';
    }

    // Gather serialized counts to rank 0
    int local_len = (int)serialized.size();
    std::vector<int> all_lens(size);
    MPI_Gather(&local_len, 1, MPI_INT, all_lens.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    std::vector<int> recv_displs(size, 0);
    int total_recv = 0;
    if (rank == 0) {
        for (int r = 0; r < size; r++) { recv_displs[r] = total_recv; total_recv += all_lens[r]; }
    }
    std::string recv_buf(total_recv, ' ');
    MPI_Gatherv(serialized.data(), local_len, MPI_CHAR,
                recv_buf.data(), all_lens.data(), recv_displs.data(), MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::map<std::string, int> global;
        std::istringstream iss3(recv_buf);
        std::string line;
        while (std::getline(iss3, line)) {
            if (line.empty()) continue;
            size_t sp = line.rfind(' ');
            if (sp == std::string::npos) continue;
            std::string word = line.substr(0, sp);
            int cnt = std::stoi(line.substr(sp+1));
            global[word] += cnt;
        }
        std::vector<std::pair<std::string,int>> result(global.begin(), global.end());
        return result;
    }
    return {};
}
