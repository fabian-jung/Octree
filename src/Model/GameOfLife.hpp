#ifndef GAMEOFLIFE_HPP
#define GAMEOFLIFE_HPP

#include <numeric>
#include "../Stencil/EightWayStencil.hpp"

template <template<class> class Container>
class GameOfLife {
public:
	typedef int NodeType;
	//AccessPattern of the model
	typedef EightWayStencil<NodeType, Container<NodeType>> AccessPattern;
	
	//CommunicationPattern Types
	typedef EightWayStencil<typename Communication::CommType, typename Container<NodeType>::CommunicationPatternMatType> CommunicationPatternAccessPattern;

public:
	static NodeType modelFunction(AccessPattern ap) {
		int neighbors = std::accumulate(ap.begin(), ap.end(), -1 * ap.center());
		if(ap.center() == 0) {
			if(neighbors == 3) return 1;
			return 0;
		} else {
			if(neighbors == 2 or neighbors == 3) return 1;
			return 0;
		}
	};
};

#endif
