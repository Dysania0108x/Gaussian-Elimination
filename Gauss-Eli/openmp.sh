#!/bin/bash -l

#Comp328 HPC
#Gaussian Elimination openmp version
#Specific course queue and max wallclock time
#SBATCH -p course -t 2
#SBATCH --exclusive
# Usage: sbatch -c N ./openmp.sh openmp_gauss.c
# Defaults on Barkla (but set to be safe)
## Specify the current working directory as the location for executables/files
#SBATCH -D ./
## Export the current environment to the compute node
#SBATCH --export=ALL

# load modules intel compiler
module load compilers/intel/2019u5 

## SLURM terms
## nodes            relates to number of nodes
## ntasks-per-node  relates to MPI processes per node
## cpus-per-task    relates to OpenMP threads (per MPI process)

echo "Node list                    : $SLURM_JOB_NODELIST"
echo "Number of nodes allocated    : $SLURM_JOB_NUM_NODES or $SLURM_NNODES"
echo "Number of threads or processes: $OMP_NUM_THREADS"
echo "Requested CPUs per task      : $SLURM_CPUS_PER_TASK"
echo "Scheduling priority          : $SLURM_PRIO_PROCESS"

# parallel using OpenMP
SRC=$1
EXE=${SRC%%.c}.exe
rm -f ${EXE}
echo compiling $SRC to $EXE
icc -qopenmp -O0 $SRC  -o $EXE
echo
echo ------------------------------------

if test -x $EXE; then
      # set number of threads
      export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1} # if '-c' not used then default to 1
      echo using ${OMP_NUM_THREADS} OpenMP threads
      echo
      echo 
      echo Multiple execution..
      echo
      echo
      # run multiple times
      # Usage: ./openmp_gauss.exe <matrix_dimension> <thread_num> 
      for i in {1..5}; 
         do 
           echo
           echo This is running process $i
           echo The size of the matrix is 500*500
           ./openmp_gauss.exe 500 $OMP_NUM_THREADS; 
       done
        
      
else
     echo $SRC did not built to $EXE
fi



