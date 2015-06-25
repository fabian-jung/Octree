#ifndef FOURWAYSTENCIL_HPP
#define FOURWAYSTENCIL_HPP

#include "../Curves/MyIterator.hpp"

#include "../Container/Octree/Octree.hpp"

template <class data, class Container> 
class FourWayStencil {
	typedef typename Container::Key Key;
	Container& mat;
	unsigned int x;
	unsigned int y;
	const unsigned int dimx;
	const unsigned int dimy;
public:
	FourWayStencil(Container& mat, const Key& key) :
		mat(mat),
		x(key.first),
		y(key.second),
		dimx(mat.getDimx()),
		dimy(mat.getDimy())
	{

	}
	Key centerKey() {
		return Key(x,y);
	}
	
	Key northKey() {
		if(y > 0) {
			return Key(x,y-1);
		} else {
			return centerKey();
		}
	}
	
	Key southKey() {
		if(y < dimy - 1) {
			return Key(x,y+1);
		} else {
			return centerKey();
		}
	}
	
	Key eastKey() {
		if(x < dimx - 1) {
			return Key(x+1,y);
		} else {
			return centerKey();
		}
	}
	Key westKey() {
		if(x > 0) {
			return Key(x-1,y);
		} else {
			return centerKey();
		}
	}
	data& center() {
		return mat[centerKey()];
	}
	data& north() {
		return mat[northKey()];
	}
	data& south() {
		return mat[southKey()];
	}
	data& east() {
		return mat[eastKey()];
	}
	data& west() {
		return mat[westKey()];
	}
	
	Key getKey() {
		return Key(x,y);
	}
	
	class iterator : public MyIterator<FourWayStencil<data, Container>, data> {
	protected:
		using MyIterator<FourWayStencil<data, Container>, data>::container;
		using MyIterator<FourWayStencil<data, Container>, data>::pos;
	public:
		iterator(FourWayStencil<data, Container>& container, unsigned int pos) :
			MyIterator<FourWayStencil<data, Container>, data>(container, pos)
		{}
		data& operator*() {
			switch(pos) {
				case 0: 
					return container.center();
				case 1: 
					return container.north();
				case 2: 
					return container.east();
				case 3: 
					return container.south();
				case 4: 
					return container.west();
				default:
					return container.center();
			}
		}
		
		Key getKey() {
			switch(pos) {
				case 0: 
					return container.centerKey();
				case 1: 
					return container.northKey();
				case 2: 
					return container.eastKey();
				case 3: 
					return container.southKey();
				case 4: 
					return container.westKey();
				default:
					return container.centerKey();
			}
		}
	};
	
	iterator begin() {
		return iterator(*this, 0);
	}
	
	iterator end() {
		return iterator(*this, 5);
	}
};

template <class data, class topology> 
class FourWayStencil<data, Octree<data, topology> > {
	typedef Octree<data, topology> Container;
	typedef typename Container::Key Key;
	Container& mat;
	unsigned int x;
	unsigned int y;
	const unsigned int dimx;
	const unsigned int dimy;
public:
	FourWayStencil(Container& mat, const Key& key) :
		mat(mat),
		x(key.first),
		y(key.second),
		dimx(mat.getDimx()),
		dimy(mat.getDimy())
	{}
	
	Key centerKey() {
		return Key(x,y);
	}
	
	Key northKey() {
		const Node<data>& center = mat.rootNode.getNode(Key(x,y));
		const unsigned int count = mat.getDimx() / (1 << center.getHeight());
		const unsigned int yoffset = centerKey().second - centerKey().second % count;
		if(yoffset > 0) {
			return Key(x,yoffset-1);
		} else {
			return centerKey();
		}
	}
	
	Key southKey() {
		const Node<data>& center = mat.rootNode.getNode(Key(x,y));
		const unsigned int count = mat.getDimx() / (1 << center.getHeight());
		const unsigned int yoffset = centerKey().second - (centerKey().second % count) + (count - 1);
		if(yoffset < dimy - 1) {
			return Key(x,yoffset+1);
		} else {
			return centerKey();
		}
	}
	
	Key eastKey() {
		const Node<data>& center = mat.rootNode.getNode(Key(x,y));
		const unsigned int count = mat.getDimx() / (1 << center.getHeight());
		const unsigned int xoffset = centerKey().first - (centerKey().first % count) + (count - 1);
		if(xoffset < dimx - 1) {
			return Key(xoffset+1,y);
		} else {
			return centerKey();
		}
	}
	Key westKey() {
		const Node<data>& center = mat.rootNode.getNode(Key(x,y));
		const unsigned int count = mat.getDimx() / (1 << center.getHeight());
		const unsigned int xoffset = centerKey().first - (centerKey().first % count);		
		if(xoffset > 0) {
			return Key(xoffset-1,y);
		} else {
			return centerKey();
		}
	}
	data& center() {
		return mat[centerKey()];
	}
	data north() {
		Node<data>& north = mat.rootNode.getNode(northKey());
		Node<data>& center = mat.rootNode.getNode(centerKey());
		if(center.getHeight() < north.getHeight()) {
			const unsigned int count = mat.getDimx() / (1 << center.getHeight());
			const int offset = northKey().first - northKey().first % count;
			data res;
			const int y = northKey().second;
			for(unsigned int x = offset; x < offset+count; x++) {
				res += mat[Key(x,y)];
			}
			res /= count;
			return res;
		} else {		
			return mat[northKey()];
		}
	}
	
	data south() {
		Node<data>& south = mat.rootNode.getNode(southKey());
		Node<data>& center = mat.rootNode.getNode(centerKey());
		if(center.getHeight() < south.getHeight()) {
			const unsigned int count = mat.getDimx() / (1 << center.getHeight());
			const int offset = southKey().first - southKey().first % count;
			data res;
			const int y = southKey().second;
			for(unsigned int x = offset; x < offset+count; x++) {
				res += mat[Key(x,y)];
			}
			res /= count;
			return res;
		} else {		
			return mat[southKey()];
		}
	}
	
	data east() {
		Node<data>& east = mat.rootNode.getNode(eastKey());
		Node<data>& center = mat.rootNode.getNode(centerKey());
		if(center.getHeight() < east.getHeight()) {
			const unsigned int count = mat.getDimy() / (1 << center.getHeight());
			int offset = eastKey().second - (eastKey().second % count);
			data res;
			const int x = eastKey().first;
			for(unsigned int y = offset; y < offset+count; y++) {
				res += mat[Key(x,y)];
			}
			res /= count;
			return res;
		} else {		
			return mat[eastKey()];
		}
	}
	
	data west() {
		Node<data>& west = mat.rootNode.getNode(westKey());
		Node<data>& center = mat.rootNode.getNode(centerKey());
		if(center.getHeight() < west.getHeight()) {
			const unsigned int count = mat.getDimy() / (1 << center.getHeight());
			int offset = westKey().second - (westKey().second % count);
			data res;
			const int x = westKey().first;
			for(unsigned int y = offset; y < offset+count; y++) {
				res += mat[Key(x,y)];
			}
			res /= count;
			return res;
		} else {		
			return mat[westKey()];
		}
	}


	Key getKey() {
		return Key(x,y);
	}
	
	class iterator : public MyIterator<FourWayStencil<data, Container>, data> {
	protected:
		using MyIterator<FourWayStencil<data, Container>, data>::container;
		using MyIterator<FourWayStencil<data, Container>, data>::pos;
	public:
		iterator(FourWayStencil<data, Container>& container, unsigned int pos) :
			MyIterator<FourWayStencil<data, Container>, data>(container, pos)
		{}
		data operator*() {
			switch(pos) {
				case 0: 
					return container.center();
				case 1: 
					return container.north();
				case 2: 
					return container.east();
				case 3: 
					return container.south();
				case 4: 
					return container.west();
				default:
					return container.center();
			}
		}
		
		Key getKey() {
			switch(pos) {
				case 0: 
					return container.centerKey();
				case 1: 
					return container.northKey();
				case 2: 
					return container.eastKey();
				case 3: 
					return container.southKey();
				case 4: 
					return container.westKey();
				default:
					return container.centerKey();
			}
		}
	};
	
	iterator begin() {
		return iterator(*this, 0);
	}
	
	iterator end() {
		return iterator(*this, 5);
	}
};

#endif
