#include <iostream>
#include <omp.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    int taskNum = 0;
    #pragma omp parallel
    {
        #pragma omp task
        {
            #pragma omp critical
            std::cout << "Hello World from task, "
                      << "executed by thread "
                      << omp_get_thread_num()
                      << " task num "
                      << taskNum++
                      << std::endl;
        }
    }
    return 0;
}
