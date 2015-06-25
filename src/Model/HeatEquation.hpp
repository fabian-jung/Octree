#ifndef HEATEQUATION_HPP
#define HEATEQUATION_HPP

#include "../Stencil/FourWayStencil.hpp"

template <template<class> class Container>
class HeatEquation {
	/* struct NodeType {
		double Heat;
	} */
public:
	typedef double NodeType;
	//AccessPattern of the model
	typedef FourWayStencil<NodeType, Container<NodeType>> AccessPattern;
	
	//CommunicationPattern Types
	typedef FourWayStencil<typename Communication::CommType, typename Container<NodeType>::CommunicationPatternMatType> CommunicationPatternAccessPattern;

private:
	static constexpr NodeType a = 0.1; //Heat coefficient
	static constexpr NodeType timestep = 0.001; // Timestep in seconds
	static constexpr NodeType dx = 0.1;  //Resolution in x-direction
	static constexpr NodeType dy = 0.1; //Resolution in y-direction
public:
	static NodeType modelFunction(AccessPattern ap) {
		NodeType center = ap.center();
		NodeType res = a*((ap.east() - 2*center + ap.west())/(dx*dx) + (ap.north() - 2*center + ap.south())/(dy*dy)) * timestep + center;
		return res;
	};
};

#endif
