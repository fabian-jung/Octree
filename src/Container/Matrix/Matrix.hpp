#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <array>
#include <iostream>
#include <boost/mpi.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include "../../Curves/MyIterator.hpp"
#include "../../Curves/LinearCurve.hpp"
#include "../CommunicationPattern.hpp"
#include "../../Curves/ParallelOffset.hpp"

template <class DataType_, unsigned int dimx_, unsigned int dimy_>
class Matrix {
public:
	typedef Matrix<Communication::CommType, dimx_, dimy_> CommunicationPatternMatType;
	typedef std::pair<unsigned int, unsigned int> Key;
	typedef DataType_ DataType;

private:
	DataType_ data[dimx_*dimy_];
	boost::mpi::communicator& world;
	const int root;
	const int chunkSize;
public:
	
	const unsigned int dimx = dimx_;
	const unsigned int dimy = dimy_;
	
	Matrix(boost::mpi::communicator& world, int root) : 
		world(world),
		root(root),
		chunkSize(dimx_*dimy_ % world.size() == 0 ? dimx_*dimy_ / world.size() : dimx_*dimy_ / world.size() + 1)
	{}
	
	class iterator : public MyIterator<Matrix<DataType, dimx_, dimy_>, DataType> {
	protected:
		using MyIterator<Matrix<DataType, dimx_, dimy_>, DataType>::container;
		using MyIterator<Matrix<DataType, dimx_, dimy_>, DataType>::pos;
	public:
		iterator(Matrix<DataType, dimx_, dimy_>& container, unsigned int pos) :
			MyIterator<Matrix<DataType, dimx_, dimy_>, DataType>(container, pos)
		{}
		DataType& operator*() {
			return container[Key(pos%dimx_, pos / dimx_)];
		}
		
		Key getKey() {
			return Key(pos%dimx_, pos / dimx_);
		}
	};
		
	DataType& get(const Key& key) {
		return data[key.first+key.second*dimx];
	}
	
	unsigned int getDimx() {
		return dimx;
	}
	
	unsigned int getDimy() {
		return dimy;
	}
	
	DataType& operator[](const Key& key) {
		return data[key.first+key.second*dimx];
	}
	
	const DataType& operator[](const Key& key) const {
		return data[key.first+key.second*dimx];
	}
	
	iterator begin() {
		return iterator(*this, 0);
	}
	
	iterator end() {
		return iterator(*this, dimx_*dimy_);
	}
	
	const unsigned int getSize() const {
		return dimx_*dimy_;
	}
	
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::make_array(data, dimx_*dimy_);
    }
    
    template <class Curve>
	void gather() {
    	const unsigned int size = dimx_*dimy_;
		const unsigned int chunkSize = size % world.size() == 0 ? size / world.size() : size / world.size() + 1;
		const unsigned int memSize = world.rank() == root ? size : chunkSize;
   		
   		std::vector<DataType> buffer(memSize);
		
		unsigned int pos = 0;
		for(auto it = Curve(*this, beginOffset(*this, world)); it != Curve(*this, endOffset(*this, world)); it++) {
			buffer[pos] = ((*it).center());
			pos++;
		}     	
		
		boost::mpi::gather(world, &buffer[0], chunkSize, buffer, root);
		
		if(world.rank() == root) {
			pos = 0;
			for(auto it = Curve(*this, 0); it != Curve(*this, size); it++) {
				(*it).center() = buffer[pos];
				pos++;
			}
		}
    }
    
    template <class Curve>
    void scatter() {
    	const unsigned int size = dimx_*dimy_;	
		const unsigned int chunkSize = size % world.size() == 0 ? size / world.size() : size / world.size() + 1;
		const unsigned int memSize = world.rank() == root ? size : chunkSize;
		
   		std::vector<DataType> buffer(memSize);
   		
   		unsigned int pos = 0;
		if(world.rank() == root) {
			for(auto it = Curve(*this, 0); it != Curve(*this, memSize); it++) {
				buffer[pos] = (*it).center();
				pos++;
			}     	
		}
		
		boost::mpi::scatter(world, buffer, &buffer[0], chunkSize, root);
		
		pos = 0;
		for(auto it = Curve(*this, beginOffset(*this, world)); it != Curve(*this, endOffset(*this, world)); it++) {
			(*it).center() = buffer[pos];
			pos++;
		}
   	}
    
    /* optimized implementation vor linear Curves */
    /*
    template <>
    void gather<LinearCurve<Matrix<DataType_, dimx_, dimy_>, FourWayStencil<DataType, Matrix<DataType_, dimx_, dimy_> > > >() {
    	const unsigned int pos = world.rank()*chunkSize;
		boost::mpi::gather(world, &data[pos], chunkSize, data, root);
    }
    
    template <>
    void scatter<LinearCurve<Matrix<DataType_, dimx_, dimy_>, FourWayStencil<DataType, Matrix<DataType_, dimx_, dimy_> > > >() {
   		const unsigned int pos = world.rank()*chunkSize;
		boost::mpi::scatter(world, data, &data[pos], chunkSize, root);
    }
    */

	void broadcast() {
		boost::mpi::broadcast(world, data, dimx_*dimy_, root);
	}

};

template <unsigned int dimx, unsigned int dimy>
struct MatrixHelper {
	template<class T> using Matrix = Matrix<T, dimx, dimy>;
};


template <class DataType, unsigned int dimx_, unsigned int dimy_>
std::ostream& operator<<(std::ostream& lhs, Matrix<DataType, dimx_, dimy_>& mat) {
	typedef typename Matrix<DataType, dimx_, dimy_>::Key Key;
	for(unsigned int y = 0; y < dimy_; y++) {
		for(unsigned int x = 0; x < dimx_; x++) {
			lhs << mat[Key(x,y)] << "	";
		}
		lhs << std::endl;
	}
	return lhs;
}
#endif
