COMPILE_CMD = "mpic++"
COMPILE_FLAGS = ["-O2", "-std=c++17", "-fopenmp"]
RUN_PREFIX = ["mpirun", "--oversubscribe", "-np", "4"]
NPROCS = 4
