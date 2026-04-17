#include <mpi.h>
#include <vector>

struct Particle { double x, y, z; int id; };

// Broadcast particles from rank 0 to all using MPI_Type_create_struct.
// All ranks return the full particles array.
std::vector<Particle> mpi_send_particles(std::vector<Particle> particles) {
    // TODO: create MPI derived type for Particle, then MPI_Bcast
    return particles;
}
