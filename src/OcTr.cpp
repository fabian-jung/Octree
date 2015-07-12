#include <algorithm>
#include <iostream>
#include <iomanip>
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
#include "Curves/HilbertCurve.hpp"

namespace mpi = boost::mpi;

#ifndef CONTAINER
	#warning "No Container specified. Using default (COctree)"
	#define CONTAINER COctree
#endif

#ifndef MODEL
	#warning "No Model specified. Using default (HeatEquation)"
	#define MODEL HeatEquation
#endif

#ifndef CURVE
	#warning "No Curve specified. Using default (HilbertCurve)"
	#define CURVE HilbertCurve
#endif

#ifndef DEPTH
	#warning "No Depth specified using default (3)"
	#define DEPTH 12
#endif

#ifndef TOPOLOGY
        #warning "No topology specified. Using default (Equidistant)"
        #define TOPOLOGY Equidistant
#endif

struct COctree {
	template<class T> using Type = Octree<T, TOPOLOGY<DEPTH> >;
};

struct CMatrix {
	static const unsigned int dim = 1 << DEPTH;
	template<class T> using Type = Matrix<T, dim, dim>;
};

typedef ConfigType< CONTAINER::Type, MODEL, CURVE > Config;


int randomGenerator() {
	return std::rand()%2;
}

int main(int argc, char** argv) {
	#ifdef __INTEL_COMPILER
	auto start = std::chrono::monotonic_clock::now();
	#else
	auto start = std::chrono::steady_clock::now();
	#endif
	
	std::srand(1);
	std::cout << std::fixed <<  std::setprecision(1);
	mpi::environment env;
	mpi::communicator world;
	const int root = 0;
    std::cout << "Init" << std::endl;	
    // Print off a hello world message
    //std::cout << "Hello from rank " << world.rank() << " out of " << world.size() << std::endl;
	world.barrier();
		
	/* Set Up */
	Config::MatType mat[2] = {Config::MatType(world, root), Config::MatType(world, root)};
	unsigned int active = 0;
	
	if(world.rank() == root) {
	/*
		std::fill(mat[active].begin(), mat[active].end(), 0);
		std::fill(mat[active].begin(), mat[active].end(), 100);
	*/
	//	std::generate(mat[active].begin(), mat[active].end(), randomGenerator);
	//	mat[active][Config::MatType::Key(3,4)] = 100;
	/*	
		double pos = 0;
		for(auto it = Config::Curve(mat[active], 0); it != Config::Curve(mat[active], mat[active].getSize()); ++it) {
			(*it).center() = pos;
			std::cout << pos << "(" << (*it).getKey().first << ", "<< (*it).getKey().second << ")" << std::endl;
			++pos;
		}
	*/
	
	//	std::cout << "Input values:" << std::endl;
	//	std::cout << mat[active] << std::endl;
	
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
	#ifdef __INTEL_COMPILER
	auto mid = std::chrono::monotonic_clock::now();
	#else
	auto mid = std::chrono::steady_clock::now();
	#endif
	std::cout << "sim" << std::endl;
	/* Simulation loop */
	for(unsigned int i = 0; i < 100; i++) {
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

	#ifdef __INTEL_COMPILER
	auto end = std::chrono::monotonic_clock::now();
	#else
	auto end = std::chrono::steady_clock::now();
	#endif
	
	auto diff1 = mid - start;
	auto diff2 = end - mid;
	auto diff3 = end - start;
	
	if(world.rank() == root) {
	
	//	std::cout << "output values:" << std::endl;
	//	std::cout << mat[active] << std::endl;
		
		std::string logFile = argv[0];
		logFile += "_";
		logFile += std::to_string(static_cast<long long>(world.size()));
		logFile += ".txt";
		
		std::ofstream Log(logFile.c_str());
		Log << "Finished " << argv[0] << " dim=" << mat[active].getDimx() << ", processes=" << world.size() << ", ";
		Log << "init: " <<  std::chrono::duration <double, std::milli>(diff1).count() << "ms, ";
		Log << "runtime: " <<  std::chrono::duration <double, std::milli>(diff2).count() << "ms, ";
	 	Log << "total: " <<  std::chrono::duration <double, std::milli>(diff3).count() << "ms" << std::endl;
		
	}
	
	/*	
	if(world.rank() == root) {
	//	std::cout << "Result:" << std::endl;
	//	std::cout << mat[active] << std::endl;
	}
	*/
	
	return 0;
}
