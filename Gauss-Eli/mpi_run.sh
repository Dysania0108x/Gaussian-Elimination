#!/bin/bash -l

#Comp328 HPC
#Gaussian Elimination mpi version
# Specific course queue and max wallclock time
#SBATCH -p course -t 2
##SBATCH -p nodes -t 2
#SBATCH --exclusive

# Defaults on Barkla (but set to be safe)
## Specify the current working directory as the location for executables/files
#SBATCH -D ./
## Export the current environment to the compute node
#SBATCH --export=ALL

# load modules
## intel compiler
module load compilers/intel 
## intel mpi wrapper and run time
module load mpi/intel-mpi/2019u5/bin


## SLURM terms
echo "Node list                    : $SLURM_JOB_NODELIST"
echo "Number of nodes allocated    : $SLURM_JOB_NUM_NODES or $SLURM_NNODES"
echo "Number of threads or processes : $SLURM_NTASKS"
echo "Number of processes per node : $SLURM_TASKS_PER_NODE"
echo "Scheduling priority          : $SLURM_PRIO_PROCESS"

# parallel using MPI
SRC=$1
EXE=${SRC%%.c}.exe
rm -f ${EXE}
echo compiling $SRC to $EXE

export numMPI=${SLURM_NTASKS:-1} # if '-n' not used then default to 1
echo 
echo 
echo compiling for $numMPI MPI processes
echo ------------------------------------------------------------------------;
mpiicc -O0 $SRC -o $EXE -lm



if test -x $EXE; then
      # run multiple times
      for i in {1..5}; 
          do 
            echo This is running process $i
            echo The size of the matrix is 500*500
            mpirun -np ${numMPI} ./${EXE} 5; 
            echo  running complete
            echo ------------------------------------------------------------------------;
            #mpirun -np 16 ./mpi.exe 100; 
      done
else
     echo $SRC did not built to $EXE
fi

