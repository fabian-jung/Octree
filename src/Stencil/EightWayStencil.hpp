#ifndef EIGHTWAYSTENCIL_HPP
#define EIGHTWAYSTENCIL_HPP

#include "../Curves/MyIterator.hpp"

template <class data, class Container> 
class EightWayStencil {
private:	
	typedef typename Container::Key Key;
	Container& mat;
	unsigned int x;
	unsigned int y;
	const unsigned int dimx;
	const unsigned int dimy;
public:
	EightWayStencil(Container& mat, const Key& key) :
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
	
	Key northEastKey() {
		int tx=x, ty=y;
		if(y > 0) ty--;
		if(x < dimx - 1) tx++;
		return Key(tx,ty);
	}
	
	Key eastKey() {
		if(x < dimx - 1) {
			return Key(x+1,y);
		} else {
			return centerKey();
		}
	}
	
	Key southEastKey() {
		int tx=x, ty=y;
		if(y < dimy - 1) ty++;
		if(x < dimx - 1) tx++;
		return Key(tx,ty);
	}

	Key southKey() {
		if(y < dimy - 1) {
			return Key(x,y+1);
		} else {
			return centerKey();
		}
	}
	
	Key southWestKey() {
		int tx=x, ty=y;
		if(y < dimy - 1) ty++;
		if(x > 0) tx--;
		return Key(tx,ty);
	}
	
	Key westKey() {
		if(x > 0) {
			return Key(x-1,y);
		} else {
			return centerKey();
		}
	}
	
	Key northWestKey() {
		int tx=x, ty=y;
		if(y > 0) ty--;
		if(x > 0) tx--;
		return Key(tx,ty);
	}
	
	data& center() {
		return mat[centerKey()];
	}
	data& north() {
		return mat[northKey()];
	}
	data& northEast() {
		return mat[northEastKey()];
	}
	data& east() {
		return mat[eastKey()];
	}
	data& southEast() {
		return mat[southEastKey()];
	}
	data& south() {
		return mat[southKey()];
	}
	data& southWest() {
		return mat[southWestKey()];
	}
	data& west() {
		return mat[westKey()];
	}
	data& northWest() {
		return mat[northWestKey()];
	}
	
	Key getKey() {
		return Key(x,y);
	}
	
	class iterator : public MyIterator<EightWayStencil<data, Container >, data> {
	protected:
		using MyIterator<EightWayStencil<data, Container>, data>::container;
		using MyIterator<EightWayStencil<data, Container>, data>::pos;
	public:
		iterator(EightWayStencil<data, Container>& container, unsigned int pos) :
			MyIterator<EightWayStencil<data, Container>, data>(container, pos)
		{}
		data& operator*() {
			switch(pos) {
				case 0: 
					return container.center();
				case 1: 
					return container.north();
				case 2: 
					return container.northEast();
				case 3: 
					return container.east();
				case 4: 
					return container.southEast();
				case 5: 
					return container.south();
				case 6: 
					return container.southWest();
				case 7: 
					return container.west();
				case 8: 
					return container.northWest();
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
					return container.northEastKey();
				case 3: 
					return container.eastKey();
				case 4: 
					return container.southEastKey();
				case 5: 
					return container.southKey();
				case 6: 
					return container.southWestKey();
				case 7: 
					return container.westKey();
				case 8: 
					return container.northWestKey();
				default:
					return container.centerKey();
			}
		}
	};
	
	iterator begin() {
		return iterator(*this, 0);
	}
	
	iterator end() {
		return iterator(*this, 9);
	}
};


#endif
