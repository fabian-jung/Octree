#!/bin/bash

FILES=bin/*.run
for CASE in ${FILES}
do
	for PROCESSES in 1 2 4 8 # 16 32
	do
		sbatch --tasks-per-node=${PROCESSES} --nodes=1 --cpus-per-task=1 --mem-per-cpu=1700 --time=00:05:00 --wrap "srun --ntasks ${PROCESSES}  ${CASE}"
	done	
done
