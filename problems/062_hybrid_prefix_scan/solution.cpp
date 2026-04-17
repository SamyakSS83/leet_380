#include <mpi.h>
#include <omp.h>
#include <vector>
#include <numeric>

std::vector<double> hybrid_prefix_scan(const std::vector<double>& local_data) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = (int)local_data.size();
    std::vector<double> result(n);

    // Phase 1: OMP local exclusive prefix scan
    // Serial exclusive scan (parallel prefix is complex; use serial for correctness)
    #pragma omp single
    {
        result[0] = 0.0;
        for (int i = 1; i < n; i++)
            result[i] = result[i-1] + local_data[i-1];
    }

    // Phase 2: MPI_Exscan to get sum of all elements on lower-rank ranks
    double local_sum = 0.0;
    for (int i = 0; i < n; i++) local_sum += local_data[i];

    double offset = 0.0;
    MPI_Exscan(&local_sum, &offset, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    // rank 0: MPI_Exscan leaves offset undefined; set to 0
    if (rank == 0) offset = 0.0;

    // Phase 3: OMP add offset to all local results
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < n; i++)
        result[i] += offset;

    return result;
}
