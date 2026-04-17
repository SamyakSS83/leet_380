#include <mpi.h>

struct ArgMax { double value; int rank; };

static void argmax_op(void* in_buf, void* inout_buf, int* len, MPI_Datatype* /*dtype*/) {
    ArgMax* in    = static_cast<ArgMax*>(in_buf);
    ArgMax* inout = static_cast<ArgMax*>(inout_buf);
    for (int i = 0; i < *len; i++) {
        if (in[i].value > inout[i].value ||
            (in[i].value == inout[i].value && in[i].rank < inout[i].rank)) {
            inout[i] = in[i];
        }
    }
}

ArgMax mpi_argmax(double local_value) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Build an MPI datatype for ArgMax
    MPI_Datatype mpi_argmax_type;
    int blocklens[2] = {1, 1};
    MPI_Aint offsets[2] = {offsetof(ArgMax, value), offsetof(ArgMax, rank)};
    MPI_Datatype types[2] = {MPI_DOUBLE, MPI_INT};
    MPI_Type_create_struct(2, blocklens, offsets, types, &mpi_argmax_type);
    MPI_Type_commit(&mpi_argmax_type);

    MPI_Op op;
    MPI_Op_create(argmax_op, /*commute=*/1, &op);

    ArgMax local  = {local_value, rank};
    ArgMax global = {0.0, 0};
    MPI_Reduce(&local, &global, 1, mpi_argmax_type, op, 0, MPI_COMM_WORLD);

    MPI_Op_free(&op);
    MPI_Type_free(&mpi_argmax_type);

    return global;
}
