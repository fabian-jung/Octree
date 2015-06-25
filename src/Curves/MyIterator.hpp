#ifndef MYITERATOR_HPP
#define MYITERATOR_HPP

#include <iterator>


template <class Container, class AccessPattern>
class MyIterator : public std::iterator<std::input_iterator_tag, AccessPattern> {
protected:
	Container& container;
	unsigned int pos;
public:
	MyIterator(Container& container) :container(container), pos(0) {}
	MyIterator(Container& container, unsigned int pos) :container(container), pos(pos) {}
	MyIterator(const MyIterator& mit) : container(mit.container), pos(mit.pos) {}
	MyIterator& operator++() {++pos;return *this;}
	MyIterator operator++(int) {MyIterator tmp(*this); operator++(); return tmp;}
	bool operator==(const MyIterator& rhs) {return pos==rhs.pos;}
	bool operator!=(const MyIterator& rhs) {return pos!=rhs.pos;}
};

#endif
