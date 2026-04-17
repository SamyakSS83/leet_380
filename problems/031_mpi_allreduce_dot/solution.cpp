#include <mpi.h>
#include <vector>

double mpi_allreduce_dot(const std::vector<double>& local_a,
                          const std::vector<double>& local_b) {
    double local_dot = 0.0;
    for (int i = 0; i < (int)local_a.size(); i++) local_dot += local_a[i] * local_b[i];
    double global_dot = 0.0;
    MPI_Allreduce(&local_dot, &global_dot, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    return global_dot;
}
