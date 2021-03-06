#!/bin/bash


#Build
make clean

for CONTAINER in COctree #C Matrix
do
	for MODEL in HeatEquation GameOfLife
	do
		for CURVE in LinearCurve ZCurve HilbertCurve
		do
			for DEPTH in 3 4 5 6 7 8 9 10 11 12 # 13 14
			do
				if [ "$CONTAINER" = "COctree" ]; then
					for TOPOLOGY in Equidistant Logarithmic 
					do						
						make ADDCFLAGS="-DCONTAINER=${CONTAINER} -DMODEL=${MODEL} -DCURVE=${CURVE} -DDEPTH=${DEPTH} -DTOPOLOGY=${TOPOLOGY}" PROGRAM_NAME=${CONTAINER}_${TOPOLOGY}_${MODEL}_${CURVE}_${DEPTH}.run BUILD=build/${CONTAINER}_${TOPOLOGY}_${MODEL}_${CURVE}_${DEPTH}
					done			         
		        	else
					make ADDCFLAGS="-DCONTAINER=${CONTAINER} -DMODEL=${MODEL} -DCURVE=${CURVE} -DDEPTH=${DEPTH}" PROGRAM_NAME=${CONTAINER}_${MODEL}_${CURVE}_${DEPTH}.run BUILD=build/${CONTAINER}_${MODEL}_${CURVE}_${DEPTH}
		        	fi
		   	 done
		done
	done
done
