#include <iostream>
#include <omp.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    #pragma omp parallel
    {
        #pragma omp single
        {
            for (int t = 0; t < omp_get_num_threads(); t++) {
            #pragma omp task
                {
                    #pragma omp critical
                    std::cout << "Hello World from task, "
                              << "executed by thread: "
                              << omp_get_thread_num()
                              << std::endl;
                }
            }
        }
    }
    return 0;
}

