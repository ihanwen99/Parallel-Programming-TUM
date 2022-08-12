#include <iostream>
#include <unistd.h>
#include <forward_list>

void process_element(int &elem) {
    usleep(1000000);
    #pragma omp critical
    std::cout << elem << std::endl;
}

void traverse_list(std::forward_list<int> &l) {
    #pragma omp parallel
    {
        #pragma omp single
        for (auto it = l.begin(); it != l.end(); it++) {
            #pragma omp task
            process_element(*it);
        }
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    std::forward_list<int> l;
    l.assign({0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

    traverse_list(l);

    return 0;
}
