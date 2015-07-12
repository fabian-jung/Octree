#ifndef EQUIDISTANT_HPP
#define EQUIDISTANT_HPP

#include "../Node.hpp"

template <int depth>
struct Equidistant {
	template <class DataType>
	void operator()(Node<DataType>* root) {
		/*
		std::cout << "depth = " << depth << std::endl;
		root->split();
		for(int i = 0; i < 4; i++) Equidistant<depth-1>()(root->children[i]);
		*/
		divide(root, depth);
	}
	
	template<class DataType>
	void divide(Node<DataType>* root, int d) {
		if(depth > 0) {
			root->split();
			for(int i = 0; i < 4; i++) divide(root->children[i], d-1);
		}
	}
};

template <>
struct Equidistant<0> {
	template <class DataType>
	void operator()(Node<DataType>* root) {}
};

#endif
