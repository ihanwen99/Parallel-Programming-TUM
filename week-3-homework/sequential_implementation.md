# Week3 - Homework

OpenMP 对应双层循环

https://stackoverflow.com/questions/13357065/how-does-openmp-handle-nested-loops

1. collapse 子句 `#pragma omp parallel for collapse(2) 效果不行啊

使用 Critical 的方式还是太慢了，得使用函数式编程的方式加速运行。

都使用 openMP 的方式。