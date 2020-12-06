#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=0:10:00

make v3_omp
./bin/v3_omp data/com-Youtube.mtx --test
