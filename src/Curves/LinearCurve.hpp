#ifndef LINEARCURVE_HPP
#define LINEARCURVE_HPP

#include "MyIterator.hpp"
#include "../Container/Octree/Octree.hpp"

template <class Container, class AccessPattern>
class LinearCurve : public MyIterator<Container, AccessPattern> {
protected:
	using MyIterator<Container, AccessPattern>::container;
	using MyIterator<Container, AccessPattern>::pos;
	
public:
	LinearCurve(Container& container) :
		MyIterator<Container, AccessPattern>(container)
	{}
	
	LinearCurve(Container& container, unsigned int pos) :
		MyIterator<Container, AccessPattern>(container, pos)
	{}
	
	AccessPattern operator*() {
		return AccessPattern(container, typename Container::Key(pos%container.getDimx(), pos/container.getDimy()));
	}
};

template <class AccessPattern, class DataType, class Topology>
class LinearCurve<Octree<DataType, Topology>, AccessPattern> : public MyIterator<Octree<DataType, Topology>, AccessPattern> {
protected:
	typedef Octree<DataType, Topology> Container;
	using MyIterator<Container, AccessPattern>::container;
	using MyIterator<Container, AccessPattern>::pos;
	unsigned int x,y,opos;
public:
	LinearCurve(Container& container) :
		MyIterator<Container, AccessPattern>(container),
		x(0),
		y(0),
		opos(0)
	{}
	
	LinearCurve(Container& container, unsigned int pos) :
		MyIterator<Container, AccessPattern>(container, pos),
		x(0),
		y(0),
		opos(0)
	{}
	
	AccessPattern operator*() {
		typedef typename Container::Key Key;
		int p = pos - opos;
		while(p>0) {
			x++;
			if(x >= container.getDimx()) {
				x = 0;
				y++;
			}
			unsigned int sdim = container.getDimx() / (1 << container.rootNode.getNode(Key(x,y)).getHeight());
			if(x%sdim == 0 and y%sdim == 0) p--;
		}
		opos = pos;
		return AccessPattern(container, typename Container::Key(x, y));
	}
};

#endif
