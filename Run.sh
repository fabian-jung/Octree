#!/bin/bash

FILES=bin/*.run
for CASE in ${FILES}
do
	for PROCESSES in 1 2 4 8 16 32
	do
		sbatch --wrap "srun --mem=2048 -n ${PROCESSES}  ${CASE}"
	done	
done
