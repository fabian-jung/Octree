#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <boost/mpi.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "Node.hpp"
#include "../CommunicationPattern.hpp"
#include "Topologies/Equidistant.hpp"
#include "Topologies/Logarithmic.hpp"
#include "../../Curves/ParallelOffset.hpp"

template <class DataType_, class Topology>
class Octree {
private:
	boost::mpi::communicator& world;
	int root;
public:
	
	typedef Octree<Communication::CommType, Topology> CommunicationPatternMatType;
	typedef std::pair<unsigned int, unsigned int> Key;
	typedef DataType_ DataType;
	
	Node<DataType> rootNode;
	
	Octree(boost::mpi::communicator& world, int root) :
		world(world),
		root(root),
		rootNode(NULL)
	{
		Topology()(rootNode);
	}
	
	unsigned int getDimx() {
		return 1 << (rootNode.getDepth() - 1);
	}
	
	unsigned int getDimy() {
		return 1 << (rootNode.getDepth() - 1);
	}
	
	class iterator : public MyIterator<Octree<DataType, Topology>, DataType> {
	protected:
		using MyIterator<Octree<DataType, Topology>, DataType>::container;
		using MyIterator<Octree<DataType, Topology>, DataType>::pos;
	public:
		iterator(Octree<DataType, Topology>& container, unsigned int pos) :
			MyIterator<Octree<DataType, Topology>, DataType>(container, pos)
		{}
		DataType& operator*() {
			return container.rootNode.getNthElemet(pos);
		}
		
		Key getKey() {
			return container.rootNode.getNthKey(pos);
		}
	};
	
	DataType& get(const Key& key) {
		return rootNode(key);
	}
	
	
	DataType& operator[](const Key& key) {
		return rootNode[key];
	}
	
	
	iterator begin() {
		return iterator(*this, 0);
	}
	
	iterator end() {
		return iterator(*this, rootNode.getSize());
	}
	
	const unsigned int getSize() const {
		return rootNode.getSize();
	}
	
	template <class Curve>
	void gather() {
    	const unsigned int size = rootNode.getSize();
		const unsigned int chunkSize = size % world.size() == 0 ? size / world.size() : size / world.size() + 1;
		const unsigned int memSize = world.rank() == root ? chunkSize*world.size() : chunkSize;
   		
   		std::vector<DataType> buffer(memSize);
		
		unsigned int pos = 0;
		for(auto it = Curve(*this, beginOffset(*this, world)); it != Curve(*this, endOffset(*this, world)); it++) {
			buffer[pos] = (*this)[(*it).getKey()];
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
    	const unsigned int size = rootNode.getSize();	
		const unsigned int chunkSize = size % world.size() == 0 ? size / world.size() : size / world.size() + 1;
		const unsigned int memSize = world.rank() == root ? size : chunkSize;
		
   		std::vector<DataType> buffer(memSize);
   		
   		unsigned int pos = 0;
		if(world.rank() == root) {
			for(auto it = Curve(*this, 0); it != Curve(*this, size); it++) {
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
    
    void broadcast() {
    	std::vector<DataType> buffer(rootNode.getSize());
    	unsigned int pos = 0;
	if(world.rank() == root) {
			for(auto it = begin(); it != end(); it++) {
				buffer[pos] = *it;
				pos++;
			}     	
    	}
    	boost::mpi::broadcast(world, &buffer[0], buffer.size(), root);
    	if(world.rank() != root) {
    		pos = 0;
			for(auto it = begin(); it != end(); it++) {
				*it = buffer[pos];
				pos++;
			}
    	}
	}
    
};

template <class Topology>
struct OctreeHelper {
	template<class T> using Octree = Octree<T, Topology>;
};


template <class DataType, class Topology>
std::ostream& operator<<(std::ostream& lhs, Octree<DataType, Topology>& rhs) {
	typedef typename Octree<DataType, Topology>::Key Key;
	
	const unsigned int dim = 1 << (rhs.rootNode.getDepth() - 1);
	for(unsigned int y = 0; y < dim; y++) {
		for(unsigned int x = 0; x < dim; x++) { 
			lhs << rhs.rootNode[Key(x,y)] << "	";
		}
		lhs << std::endl;
	}
	return lhs;
}
#endif
