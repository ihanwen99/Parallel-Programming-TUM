#include <iostream>
#include <omp.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    #pragma omp parallel
    {
        #pragma omp task
        {
            #pragma omp critical
            std::cout << "Hello World from task" << std::endl;
        }
    }
    return 0;
}
