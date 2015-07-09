#ifndef HILBERTCURVE_HPP
#define HILBERTCURVE_HPP

#include "MyIterator.hpp"

template <class Container, class AccessPattern>
class HilbertCurve {};

template<class DataType, class Topology, class AccessPattern>
class HilbertCurve<Octree<DataType, Topology>, AccessPattern> : public MyIterator<Octree<DataType, Topology>, AccessPattern> {
protected:
	typedef Octree<DataType, Topology> Container;
	using MyIterator<Container, AccessPattern>::container;
	using MyIterator<Container, AccessPattern>::pos;
	struct Orientation {
		enum OrientationEnum {
			//Clockwise enumeration of all orientations
			north,
			east,
			south,
			west,
			directions
		};
		
		static OrientationEnum left(OrientationEnum in) {
			return static_cast<OrientationEnum>((in + 3) % directions); 
		}
		
		static OrientationEnum right(OrientationEnum in) {
			return static_cast<OrientationEnum>((in + 1) % directions);
		}
		
		static OrientationEnum mirrorX(OrientationEnum id) {
			if(id % 2) return static_cast<OrientationEnum>(id-1);
			return static_cast<OrientationEnum>(id+1);
		}
		
		static OrientationEnum mirrorY(OrientationEnum id) {
			if(id > 2) return static_cast<OrientationEnum>(id-2);
			return static_cast<OrientationEnum>(id+2);
		}
		static OrientationEnum getCurveRotation(OrientationEnum in, unsigned int subTreeId, unsigned int height) {
			if(in % 2 == 0) {
				if(subTreeId == 0) return right(in);
				if(subTreeId == 3) return left(in);
			} else {
				if(subTreeId == 0) return left(in);
				if(subTreeId == 3) return right(in);
			}
			return in;
		}
		
		static OrientationEnum rotateClockwise(OrientationEnum in, unsigned int steps) {
			return static_cast<OrientationEnum>((in + steps) % directions);
		}
	};
	unsigned int getEastId(unsigned int child) {
		if(child < 2) return child ;
		if(child == 2) return 3;
		return 2;
	}
	
	unsigned int mirrorId(unsigned int id) {
		if(id < 2) return id+2;
		return id-2;
	}
	
	Node<DataType>* getSubTree(Node<DataType>* parent, unsigned int childId, typename Orientation::OrientationEnum orientation) {
		const unsigned int steps = (Orientation::directions + orientation - Orientation::east) % Orientation::directions;
		unsigned int id = getEastId((childId + steps) % Orientation::directions);
		if(orientation == 1 or orientation == 3) id = mirrorId(id);
		return parent->children[id];
	}
	
	Node<DataType>* getLeaf(Node<DataType>* tree, unsigned int index, typename Orientation::OrientationEnum orientation) {
		if(tree->getDepth() == 1) return tree;
		for(unsigned int subTreeId = 0; subTreeId < Orientation::directions; subTreeId++) {
			Node<DataType>* subTree = getSubTree(tree, subTreeId, orientation);
			unsigned int subTreeSize = subTree->getSize();
			if(index < subTreeSize) {
				return getLeaf(subTree, index, Orientation::getCurveRotation(orientation, subTreeId, subTree->getHeight()));
			} else {
				index -= subTreeSize;
			}
		}
		//Unreachable statement
		return NULL;
	}
	
	
public:
	HilbertCurve(Container& container) :
		MyIterator<Container, AccessPattern>(container)
	{}
	
	HilbertCurve(Container& container, unsigned int pos) :
		MyIterator<Container, AccessPattern>(container, pos)
	{}
	
	AccessPattern operator*() {
		const Node<DataType>* leaf = getLeaf(&container.rootNode, pos, Orientation::south);	
		return AccessPattern(container, leaf->getKey(container.rootNode.getDepth()));
	}
};

template<class DataType, unsigned int dimx, unsigned int dimy, class AccessPattern>
class HilbertCurve<Matrix<DataType, dimx, dimy>, AccessPattern> : public MyIterator<Matrix<DataType, dimx, dimy>, AccessPattern> {
protected:
	typedef Matrix<DataType, dimx, dimy> Container;
	using MyIterator<Container, AccessPattern>::container;
	using MyIterator<Container, AccessPattern>::pos;
	typedef typename Container::Key Key;
	
	//rotate/flip a quadrant appropriately
	Key rot(int n, Key key, int rx, int ry) {
		if (ry == 0) {
		    if (rx == 1) {
		        key.first = n-1 - key.first;
		        key.second = n-1 - key.second;
		    }

		    //Swap x and y
		    return Key(key.second, key.first);
		}
		return key;
	}
	
	//convert pos to Key(x,y)
	Key posToKey(int pos) {
		int rx, ry, t=pos;
		Key res(0,0);
		for (unsigned int s = 1; s < dimx; s*=2) {
		    rx = 1 & (t/2);
		    ry = 1 & (t ^ rx);
		   	res = rot(s, res, rx, ry);
		    res.first += s * rx;
		    res.second += s * ry;
		    t /= 4;
		}
		return res;
	}
	
public:
	HilbertCurve(Container& container) :
		MyIterator<Container, AccessPattern>(container)
	{}
	
	HilbertCurve(Container& container, unsigned int pos) :
		MyIterator<Container, AccessPattern>(container, pos)
	{}
	
	AccessPattern operator*() {
		return AccessPattern(container, posToKey(pos));
	}
};

#endif
