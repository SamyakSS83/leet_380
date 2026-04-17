#include <mpi.h>
#include <vector>
#include <string>

// Each rank contributes "Hello from rank R of S".
// Return sorted messages on rank 0; return empty on other ranks.
std::vector<std::string> mpi_hello(int argc, char** argv) {
    // TODO: implement using MPI_Gather
    return {};
}
