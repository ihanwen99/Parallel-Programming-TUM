#include <cstdio>
#include <cstdlib>

int fib(int n) {
    int i, j;

    if (n < 2) return n;

    #pragma omp task shared(i) firstprivate(n)
    i = fib(n - 1);

    #pragma omp task shared(j) firstprivate(n)
    j = fib(n - 2);

    #pragma omp taskwait
    return i + j;
}

int main(int argc, char **argv) {
    int n = 45;

    if (argc > 1)
        n = atoi(argv[1]);

    #pragma omp parallel shared(n)
    {
        #pragma omp single
        printf("fib(%d) = %d\n", n, fib(n));
    }
}
