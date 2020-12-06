#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=0:10:00

make v3
./bin/v3 data/mycielskian13.mtx
