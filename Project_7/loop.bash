#!/bin/bash
#SBATCH -J fourier.cpp
#SBATCH -A cs475-575
#SBATCH -p classmpitest
#SBATCH -N 8 # number of nodes
#SBATCH -n 8 # number of tasks
#SBATCH --constraint=ib
#SBATCH -o fourier.out
#SBATCH -e fourier.err
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=nalubanr@oregonstate.edu

module  load  openmpi
mpic++ fourier.cpp -o fourier -lm
for n in 1 2 4 6 8
do
mpiexec -mca btl self,tcp -np $n ./fourier
done
