#ifndef PARALLELOFFSET_HPP
#define PARALLELOFFSET_HPP

#include <boost/mpi.hpp>

/*
const unsigned int beginOffset = size *    world.rank()  / world.size();
const unsigned int endOffset   = size * (1+world.rank()) / world.size();
const unsigned int chunkSize = size % world.size() == 0 ? size / world.size() : size / world.size() + 1;
*/

template <class Container>
unsigned int beginOffset(const Container& container, int rank, boost::mpi::communicator& world) {
	const unsigned int count =  static_cast<float>(container.getSize()) / world.size() + 0.5;
	return count*world.rank();
}

template <class Container>
unsigned int endOffset(const Container& container, int rank, boost::mpi::communicator& world) {
	const unsigned int count =  static_cast<float>(container.getSize()) / world.size() + 0.5;	
	return count*(world.rank()+1) < container.getSize() ? count*(world.rank()+1) : container.getSize();
}

template <class Container>
inline unsigned int beginOffset(const Container& container, boost::mpi::communicator& world) {
	return beginOffset(container, world.rank(), world);
}

template <class Container>
inline unsigned int endOffset(const Container& container, boost::mpi::communicator& world) {
	return endOffset(container, world.rank(), world);
}

#endif

