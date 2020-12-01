#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=0:20:00

make v4_pthreads
./bin/v4_pthreads data/com-Youtube.mtx --test
