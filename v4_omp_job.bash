#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=0:20:00

make v4_omp
./bin/v4_omp data/com-Youtube.mtx --test
