#include <mpi.h>
#include <omp.h>
#include <vector>
#include <climits>
#include <cfloat>

struct MinLoc {
    double value;
    int rank;
    int local_index;
};

// MPI custom reduction operator for MinLoc
static void minloc_op(void* in, void* inout, int* len, MPI_Datatype* dtype) {
    (void)dtype;
    MinLoc* a = (MinLoc*)in;
    MinLoc* b = (MinLoc*)inout;
    for (int i = 0; i < *len; i++) {
        bool a_wins = (a[i].value < b[i].value) ||
                      (a[i].value == b[i].value && a[i].rank < b[i].rank) ||
                      (a[i].value == b[i].value && a[i].rank == b[i].rank &&
                       a[i].local_index < b[i].local_index);
        if (a_wins) b[i] = a[i];
    }
}

MinLoc hybrid_global_min(const std::vector<double>& local_data) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = (int)local_data.size();

    // OMP parallel reduction to find local min and its index
    double local_min = local_data[0];
    int local_idx = 0;

    #pragma omp parallel
    {
        double tmin = DBL_MAX;
        int tidx = 0;
        #pragma omp for schedule(static) nowait
        for (int i = 0; i < n; i++) {
            if (local_data[i] < tmin) {
                tmin = local_data[i];
                tidx = i;
            }
        }
        #pragma omp critical
        {
            if (tmin < local_min || (tmin == local_min && tidx < local_idx)) {
                local_min = tmin;
                local_idx = tidx;
            }
        }
    }

    MinLoc local_ml = {local_min, rank, local_idx};

    // Build MPI datatype for MinLoc
    MPI_Datatype mpi_minloc;
    int block_lens[2] = {1, 2};
    MPI_Aint disps[2] = {0, sizeof(double)};
    MPI_Datatype types[2] = {MPI_DOUBLE, MPI_INT};
    MPI_Type_create_struct(2, block_lens, disps, types, &mpi_minloc);
    MPI_Type_commit(&mpi_minloc);

    // Create custom MPI op
    MPI_Op mpi_op;
    MPI_Op_create(minloc_op, 1, &mpi_op);

    MinLoc global_ml = {0.0, 0, 0};
    MPI_Reduce(&local_ml, &global_ml, 1, mpi_minloc, mpi_op, 0, MPI_COMM_WORLD);

    MPI_Op_free(&mpi_op);
    MPI_Type_free(&mpi_minloc);

    if (rank == 0) return global_ml;
    return {0.0, 0, 0};
}
