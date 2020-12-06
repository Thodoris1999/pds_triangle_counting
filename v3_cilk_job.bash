#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --time=0:07:00

make v3_cilk

mat=data/dblp-2010.mtx

for i in {1..32}
do
	export CILK_NWORKERS=$i
	./bin/v3_cilk $mat
done
