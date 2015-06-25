#ifndef LOGARITHMIC_HPP
#define LOGARITHMIC_HPP

#include "../Node.hpp"

template <int depth>
struct Logarithmic {
	template <class DataType>
	void operator()(Node<DataType>& root) {
		root.split();
		for(int i = 0; i < 2; i++) Logarithmic<depth-1>()(*root.children[i+2]);
	}
};

template <>
struct Logarithmic<0> {
	template <class DataType>
	void operator()(Node<DataType>& root) {}
};


#endif
