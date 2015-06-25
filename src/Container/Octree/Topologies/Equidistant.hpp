#ifndef EQUIDISTANT_HPP
#define EQUIDISTANT_HPP

#include "../Node.hpp"

template <int depth>
struct Equidistant {
	template <class DataType>
	void operator()(Node<DataType>& root) {
		root.split();
		for(int i = 0; i < 4; i++) Equidistant<depth-1>()(*root.children[i]);
	}
};

template <>
struct Equidistant<0> {
	template <class DataType>
	void operator()(Node<DataType>& root) {}
};

#endif
