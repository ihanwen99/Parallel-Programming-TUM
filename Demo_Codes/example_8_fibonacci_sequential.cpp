#include <cstdio>
#include <cstdlib>

int fib(int n) {
    int i, j;

    if (n < 2) return n;

    i = fib(n - 1);
    j = fib(n - 2);

    return i + j;
}

int main(int argc, char **argv) {
    int n = 45;

    if (argc > 1)
        n = atoi(argv[1]);

    printf("fib(%d) = %d\n", n, fib(n));
}
