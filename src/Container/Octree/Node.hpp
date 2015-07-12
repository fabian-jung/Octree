#ifndef NODE_HPP
#define NODE_HPP

#include <memory>
#include <iostream>
#include <algorithm>

/*

	Octree Node
	
	|-------|-------|
	|		|		|	
	|	0	|	1	|
	|	 	|		|
	|-------|-------|
	|		|		|
	|	2	|	3	|
	|		|		|
	|-------|-------|

	|-------|-------|
	| 0	| 1	|		|	
	|---|---|	1	|
	| 2	| 3	|		|
	|-------|-------|
	|		|		|
	|	2	|	3	|
	|		|		|
	|-------|-------|

*/


template <class DataType>
class Node {
private:
	unsigned int size;
	unsigned int depth;
	unsigned int height;
	bool leaf;
	Node* parent;
	DataType* value;

public:
	Node* children[4];

	typedef std::pair<unsigned int, unsigned int> Key;

	Node(Node* parent) :
		size(1),
		depth(1),
		height(parent != NULL ? parent->height+1 : 0),
		leaf(true),
		parent(parent),
		value(new DataType())
	{}
	
	Node() : Node(NULL) {
	
	}
	
	void nDelete() {
		if(value != NULL) {
			delete value;
		} else {
			for(unsigned int i = 0; i < 4; i++) {
				children[i]->nDelete();
				delete children[i];
			}
		}
	}
	void split() {
		if(leaf) {
			leaf = false;
			for(unsigned int i = 0; i < 4; i++) {
				children[i] = new Node(this);
			}
			delete value;
			increaseSize(3);
			depth++;
			if(parent != NULL) parent->updateDepth();
		}
	}
	void increaseSize(unsigned int inc) {
		size += 3;
		if(parent != NULL) parent->increaseSize(3);
	}
	
	void updateDepth() {
		const unsigned int oldDepth = depth;
		if(!leaf) depth = 1 + std::max(std::max(children[0]->depth, 
									            children[1]->depth),
									   std::max(children[2]->depth,
									            children[3]->depth)
									  );
		if(depth != oldDepth and parent != NULL) parent->updateDepth();
	}
	
	DataType& operator[](Key key) {
		if(leaf) return *value;
		const unsigned int dim = 1 << (depth-1);
		if(key.first < dim/2) {
			if(key.second < dim/2) {
				return (*children[0])[key];
			} else {
				key.second -= dim/2;
				return (*children[2])[key];
			}
		} else {
			key.first -= dim/2;
			if(key.second < dim/2) {
				return (*children[1])[key];
			} else {
				key.second -= dim/2;
				return (*children[3])[key];
			}
		}
	}
	
	unsigned int getSize() const {
		return size;
	}
	
	unsigned int getDepth() const {
		return depth;
	}
	
	unsigned int getHeight() const {
		return height;
	}
	
	Key getNthKey(unsigned int n) const {
		if(leaf) return Key(0,0);
		unsigned int i;
		for(i = 0; i < 4; i++) {
			if(n >= children[i]->size) n -= children[i]->size;
			else break;
		}
		Key res = children[i]->getNthKey(n);
		if(i == 1 || i == 3) {
			res.first += 1 << (depth - 2);
		}
		if(i == 2 || i == 3) {
			res.second += 1 << (depth - 2);
		}
		return res;
	}

	DataType& getNthElemet(unsigned int n) {
		if(leaf) return *value;
		unsigned int i;
		for(i = 0; i < 4; i++) {
			if(n >= children[i]->size) n -= children[i]->size;
			else break;
		}
		return children[i]->getNthElemet(n);
	}

	Node& getNode(Key key) {
		if(leaf) return *this;
		const unsigned int dim = 1 << (depth-1);
		if(key.second < dim/2) {
			if(key.first < dim/2) {
				return (children[0]->getNode(key));
			} else {
				key.first -= dim/2;
				return (children[1]->getNode(key));
			}
		} else {
			key.second -= dim/2;
			if(key.first < dim/2) {
				return (children[2]->getNode(key));
			} else {
				key.first -= dim/2;
				return (children[3]->getNode(key));
			}
		}
	}
	
	Key getKey(const unsigned int maxDepth) const {
		if(parent == NULL) return Key(0,0);
		Key res = parent->getKey(maxDepth);
		int childId = std::find(parent->children, parent->children+4, this) - parent->children;
		if(childId >= 2) {
			res.second +=  1 << (maxDepth - height - 1);
			childId -= 2;
		}
		if(childId == 1) {
			res.first +=  1 << (maxDepth - height - 1);
		}
		return res;
	}

};

#endif
