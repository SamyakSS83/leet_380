#include <mpi.h>
#include <vector>
#include <cstddef>

struct Particle { double x, y, z; int id; };

std::vector<Particle> mpi_send_particles(std::vector<Particle> particles) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Build MPI type for Particle
    int count = 4;
    int blocklens[] = {1, 1, 1, 1};
    MPI_Aint offsets[4];
    offsets[0] = offsetof(Particle, x);
    offsets[1] = offsetof(Particle, y);
    offsets[2] = offsetof(Particle, z);
    offsets[3] = offsetof(Particle, id);
    MPI_Datatype types[] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
    MPI_Datatype particle_type;
    MPI_Type_create_struct(count, blocklens, offsets, types, &particle_type);
    MPI_Type_commit(&particle_type);

    int n = 0;
    if (rank == 0) n = (int)particles.size();
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) particles.resize(n);
    MPI_Bcast(particles.data(), n, particle_type, 0, MPI_COMM_WORLD);

    MPI_Type_free(&particle_type);
    return particles;
}
