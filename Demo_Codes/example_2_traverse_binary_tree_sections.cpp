#include <stdio.h>
#include "utility.h"

//struct node
//{
//	struct node *left, *right;
//	int key;
//
//	node(int k):key(k){}
//};

void process(Node *p) {
    usleep(1000000);
#pragma omp critical
    std::cout << "Element with key "
              << p->key << " is processed"
              << std::endl;
}

void traverse(Node *p) {
    process(p);

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            if (p->left != nullptr)
                traverse(p->left);
        }

        #pragma omp section
        {
            if (p->right != nullptr)
                traverse(p->right);
        }
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    Node *tree = generateTree(); // Initialize tree

    traverse(tree);
    return 0;
}
