#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <random>

struct node
{
	struct node *left, *right;
	int key;

	node(int k) : key(k) {
	    left = nullptr;
	    right = nullptr;
	}
};

typedef struct node Node;

Node* generateNode(int depth) {
    static int nodeIndex = 0;

    std::cout << "Generate node " << nodeIndex << std::endl;
	Node* node = new Node(nodeIndex++);
	if(depth < 3) {
		node->left = generateNode(depth + 1);
	}
	if(depth < 3) {
		node->right = generateNode(depth + 1);
	}
	return node;
}

Node* generateTree() {
	return generateNode(0);
}
