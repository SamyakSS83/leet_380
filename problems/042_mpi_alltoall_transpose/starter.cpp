#include <mpi.h>
#include <vector>

// Distributed transpose of an n x n matrix.
// local_rows: (n/size) rows of A, flat row-major, length = (n/size)*n
// Returns: columns [rank*block .. (rank+1)*block) of A, row-major, length = n*(n/size)
std::vector<double> distributed_transpose(const std::vector<double>& local_rows, int n) {
    // TODO: implement
    return {};
}
