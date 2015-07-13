#ifndef LOGARITHMIC_HPP
#define LOGARITHMIC_HPP

#include "../Node.hpp"

template <int depth>
struct Logarithmic {
	template <class DataType>
	void operator()(Node<DataType>* root) {
		/*
		root->split();
		for(int i = 0; i < 2; i++) Logarithmic<depth-1>()(root->children[i+2]);
		*/
		divide(root, depth);
	}
	
	template<class DataType>
	void divide(Node<DataType>* root, int d) {
		if(d > 0) {
			root->split();
			for(int i = 0; i < 2; i++) divide(root->children[i], d-1);
		}
	}
};

template <>
struct Logarithmic<0> {
	template <class DataType>
	void operator()(Node<DataType>* root) {}
};


#endif
