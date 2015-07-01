#ifndef COMMUNICATIONPATTERN_HPP
#define COMMUNICATIONPATTERN_HPP

#include <iostream>
#include <boost/mpi.hpp>
#include <vector>
#include <map>
#include <boost/serialization/serialization.hpp>
#include "../Curves/ParallelOffset.hpp"

namespace Communication {
	typedef int CommType;
};

template <class Container, class AccessPattern, class Curve>
class CommunicationPattern {
private:

	boost::mpi::communicator& world;
	const int root;
	
	typedef typename Container::Key Key;	
	typedef std::map<unsigned int, std::vector<Key> > SendList;
	typedef SendList ReceiveList;
	
	SendList    sendList;
	ReceiveList receiveList;
	
	
	void printSendList(const SendList& rhs) {
		for(const std::pair<unsigned int, std::vector<Key> > kv : rhs) {
			std::cout << kv.first << ": " << std::endl;
			for(Key k : kv.second) {
				std::cout << "(" << k.first << ", " << k.second << ") ";
			} 
			std::cout << std::endl;
		}
	}
	
	template <class Buffer>
	void printBuffer(const Buffer& buffer) {
		std::cout << "[";
		for(unsigned int i = 0; i < buffer.size(); i++) {
			std::cout << buffer[i];
			if(i < buffer.size() - 1) std::cout << ", ";
		}
		std::cout << "]" << std::endl;
	}
	
	
public:
	CommunicationPattern(boost::mpi::communicator& world, int root) :
		world(world),
		root(root)
	{
		Container container(world, root); //Comm from node id to node id 
		
		for(auto it = Curve(container, beginOffset(container, world)); it != Curve(container, endOffset(container, world)); ++it) {
			container[(*it).getKey()] = Communication::CommType(world.rank());
		}
		container.template gather<Curve>();
		//Broadcast all send and receive operations
		
		/*
		if(world.rank() == 0) {
			std::cout << "Partition:" << std::endl;
			std::cout << container << std::endl;
		}
		*/
		
		container.broadcast();
		
		//Every Process creates his list of send and receive operations
		for(typename Container::iterator i = container.begin(); i != container.end(); ++i) {
			
			if((*i) == world.rank()) {
				auto ap = AccessPattern(container, i.getKey());
				for(typename AccessPattern::iterator n = ap.begin(); n != ap.end(); ++n) {
					if(container[n.getKey()] != world.rank()) { //neighbor is on different process
						sendList[container[n.getKey()]].push_back(i.getKey());
					}
				}
			} else {
				auto ap = AccessPattern(container, i.getKey());
				for(typename AccessPattern::iterator n = ap.begin(); n != ap.end(); ++n) {
					if(container[n.getKey()] == world.rank()) { //neighbor is me
						receiveList[container[i.getKey()]].push_back(i.getKey());
					}
				}
			}
			
		}
		/*
		world.barrier();
		for(int i = 0; i < world.size(); i++) {
			if(world.rank() == i) {
				std::cout << "SendList rank " << i << ":" << std::endl;
				printSendList(sendList);
				std::cout << "ReceiveList rank " << i << ":" << std::endl;
				printSendList(receiveList);
			}
			world.barrier();
		}
		*/
	}
	
	template <class Container_>
	void communicate(Container_& con) {
	//Prepare sendbuffer
		typedef typename Container_::DataType DataType;
		typedef std::vector<DataType> Buffer;
		std::map<int, Buffer> sendBuffer, receiveBuffer;
		
		for(auto sendObject : sendList) {
			for(auto k :sendObject.second) {
				sendBuffer[sendObject.first].push_back(con[k]);
			}
		}
		for(auto receiveObject : receiveList) {
			receiveBuffer[receiveObject.first] = Buffer(receiveObject.second.size());
		}
		
		//List of all requests
		//auto requestQueue = new boost::mpi::request[sendBuffer.size() + receiveBuffer.size()];
		std::vector<boost::mpi::request> requestQueue;
		//unsigned int pos = 0;
		//All send operations
		for(auto i : sendBuffer) {
			//std::cout << "rank " << world.rank() << " sends to " << i.first << " tag " << world.rank() << " some data with size " << i.second.size() << std::endl;
			requestQueue.push_back(world.isend(i.first, world.rank(), &i.second[0], i.second.size()));
		}
		
		//All receive operations
		for(auto i : receiveBuffer) {
			//std::cout << "rank " << world.rank() << " receives from " << i.first << " tag " << i.first << " some data with size " << receiveList[i.first].size() << std::endl;
			requestQueue.push_back(world.irecv(i.first, i.first, &receiveBuffer[i.first][0], receiveList[i.first].size()));
		}

		//Synchronize
		boost::mpi::wait_all(requestQueue.begin(), requestQueue.end());
		//std::cout << world.rank() << " all send/receive complete" << std::endl;

		//delete[] requestQueue;
		/*
		world.barrier();
		for(int i = 0; i < world.rank(); i++) {
			if(world.rank() == i) {
				std::cout << "SendBuffer rank " << i << ":" << std::endl;
				printBuffer(sendBuffer[0]);
				std::cout << "ReceiveBuffer rank " << i << ":" << std::endl;
				printBuffer(receiveBuffer[0]);
			
			}
			world.barrier();
		}
		*/
		//Move values to the right cells
		for(auto receiveObject : receiveBuffer) {
			int i = 0;
			for(DataType d : receiveObject.second) {
				con[receiveList[receiveObject.first][i]] = d;
				i++;
			}
		}
		//Ready for the next calculation iteration
	}
};
#endif
