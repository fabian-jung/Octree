#ifndef ZCURVE_HPP
#define ZCURVE_HPP

#include "MyIterator.hpp"

template <class Container, class AccessPattern>
class ZCurve {};

template<class DataType, class Topology, class AccessPattern>
class ZCurve<Octree<DataType, Topology>, AccessPattern> : public MyIterator<Octree<DataType, Topology>, AccessPattern> {
protected:
	typedef Octree<DataType, Topology> Container;
	using MyIterator<Container, AccessPattern>::container;
	using MyIterator<Container, AccessPattern>::pos;
	
public:
	ZCurve(Container& container) :
		MyIterator<Container, AccessPattern>(container)
	{}
	
	ZCurve(Container& container, unsigned int pos) :
		MyIterator<Container, AccessPattern>(container, pos)
	{}
	
	AccessPattern operator*() {
		return AccessPattern(container,  container.rootNode.getNthKey(pos));
	}
};

template<class DataType, unsigned int dimx, unsigned int dimy, class AccessPattern>
class ZCurve<Matrix<DataType, dimx, dimy>, AccessPattern> : public MyIterator<Matrix<DataType, dimx, dimy>, AccessPattern> {
protected:
	typedef Matrix<DataType, dimx, dimy> Container;
	typedef typename Container::Key Key;
	using MyIterator<Container, AccessPattern>::container;
	using MyIterator<Container, AccessPattern>::pos;
	
public:
	ZCurve(Container& container) :
		MyIterator<Container, AccessPattern>(container)
	{}
	
	ZCurve(Container& container, unsigned int pos) :
		MyIterator<Container, AccessPattern>(container, pos)
	{}
	
	unsigned int log2(unsigned int i) {
		int log = 0;
		while(i > 1) {
			i/=2;
			log++;
		}
		return log;
	}
	
	Key getNthKey(unsigned int n, int depth) {
		if(n==0) return(Key(0,0));
		unsigned int dim = 1 << depth;
		unsigned int x = 0, y = 0;
		if(n >= dim*dim/2) {
			y += dim/2;
			n -= dim*dim/2;
			
		} 
		if(n >= dim*dim/4 ) {
			x += dim/2;
			n -= dim*dim/4;
		}
		Key res = getNthKey(n, depth-1);
		return Key(res.first+x, res.second+y);
	}
	
	AccessPattern operator*() {
		return AccessPattern(container,  getNthKey(pos,log2(dimx)));
	}
};

#endif
