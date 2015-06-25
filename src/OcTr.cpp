#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <boost/mpi.hpp>
#include <chrono>
#include <fstream>
#include <string>

#include "Config.hpp"
#include "Container/Matrix/Matrix.hpp"
#include "Container/Octree/Octree.hpp"
#include "Model/HeatEquation.hpp"
#include "Model/GameOfLife.hpp"
#include "Curves/LinearCurve.hpp"
#include "Curves/ZCurve.hpp"


namespace mpi = boost::mpi;

#ifndef CONTAINER
	#define CONTAINER Octree
#endif

#ifndef MODEL
	#define MODEL HeatEquation
#endif

#ifndef CURVE
	#define CURVE LinearCurve
#endif

#ifndef DEPTH
	#define DEPTH 3
#endif

#if CONTAINER == Octree
	#ifndef TOPOLOGY
		#define TOPOLOGY Equidistant
	#endif
	typedef ConfigType<OctreeHelper<TOPOLOGY<DEPTH> >::Octree, MODEL, CURVE > Config;
#else
#if CONTAINER == Matrix 
	const int dim = 1 << DEPTH;
	typedef ConfigType<MatrixHelper<dim, dim>::Matrix , MODEL, CURVE > Config;
#endif
#endif

int randomGenerator() {
	return std::rand()%2;
}

int main(int argc, char** argv) {
	auto start = std::chrono::steady_clock::now();

	std::srand(1);
	std::cout << std::fixed;
	mpi::environment env;
	mpi::communicator world;
	const int root = 0;
	
    // Print off a hello world message
    std::cout << "Hello from rank " << world.rank() << " out of " << world.size() << std::endl;
	world.barrier();
		
	/* Set Up */
	Config::MatType mat[2] = {Config::MatType(world, root), Config::MatType(world, root)};
	unsigned int active = 0;
	
	if(world.rank() == root) {
	/*
		std::fill(mat[active].begin(), mat[active].end(), 0);
		std::fill(mat[active].begin(), mat[active].end(), 100);
		std::generate(mat[active].begin(), mat[active].end(), randomGenerator);
	*/
		mat[active][Config::MatType::Key(3,4)] = 100;
		
	/*
		double pos = 0;
		for(auto it = Config::Curve(mat[active], 0); it != Config::Curve(mat[active], mat[active].getSize()); ++it) {
			(*it).center() = pos;
			++pos;
		}
	*/
	/*
		std::cout << "Input values:" << std::endl;
		std::cout << mat[active] << std::endl;
	*/
	}

	Config::CommunicationPatternType commPattern(world, root);
	
	//mat[active].broadcast();
	mat[active].scatter<Config::Curve>();
	commPattern.communicate(mat[active]);
	
	/*
	for(int i = 0; i < world.size(); i++) {
		if(world.rank() == i) {
			std::cout << "Input values for rank " << i << ":" << std::endl;
			std::cout << mat[active] << std::endl;
		}
		world.barrier();
	}
	*/

	auto mid = std::chrono::steady_clock::now();

	/* Simulation loop */
	for(unsigned int i = 0; i < 1000; i++) {
		//Calc new Values
		for(auto it = Config::Curve(mat[active], beginOffset(mat[active], world)); it != Config::Curve(mat[active], endOffset(mat[active], world)); ++it) {
			//std::cout << (*it).getKey().first << ", " << (*it).getKey().second << " = " << mat[active][(*it).getKey()] << std::endl;
			mat[!active][(*it).getKey()] = Config::ModelType::modelFunction(*it);
			//std::cout << mat[!active][(*it).getKey()] << std::endl;
		}
	
		//Communicate boarders
		commPattern.communicate(mat[!active]);

		/*
		mat[!active].gather<Config::Curve>();
		mat[!active].broadcast();
		*/
		//Swap buffers
		active = !active;	
	}

	mat[active].gather<Config::Curve>();

	auto end = std::chrono::steady_clock::now();
	auto diff1 = mid - start;
	auto diff2 = end - mid;
	auto diff3 = end - start;
	
	if(world.rank() == root) {
		std::string logFile = argv[0];
		logFile += ".txt";
		
		std::ofstream Log(logFile.c_str());
		Log << "Finished " << argv[0] << " dim=" << mat[active].getDimx() << ", processes=" << world.size() << ", ";
		Log << "init: " <<  std::chrono::duration <double, std::milli>(diff1).count() << "ms, ";
		Log << "runtime: " <<  std::chrono::duration <double, std::milli>(diff2).count() << "ms, ";
	 	Log << "total: " <<  std::chrono::duration <double, std::milli>(diff3).count() << "ms" << std::endl;
		
	}
	
	/*	
	if(world.rank() == root) {
		std::cout << "Result:" << std::endl;
		std::cout << mat[active] << std::endl;
	}
	*/
	
	return 0;
}
