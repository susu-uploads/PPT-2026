#!/bin/bash
#SBATCH --gres=gpu:1
nvcc task.cu -o task.o
./task.o
rm task.o

