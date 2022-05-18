# Week3 - Homework

```cpp
int pointsInSetCount = 0;
// For each pixel in the image
#pragma omp parallel for schedule(dynamic) reduction(+: pointsInSetCount)
for (int i = 0; i < y_resolution; i++)
{
  for (int j = 0; j < x_resolution; j++)
  {
    double y = VIEW_Y1 - i * y_stepsize;
    double x = VIEW_X0 + j * x_stepsize;

    std::complex<double> Z = 0.0 + 0.0i;
    std::complex<double> C = x + y * 1.0i;

    int k = 0;

    do
    {
      Z = std::pow(Z, power) + C;
      k++;
    } while (std::abs(Z) < 2 && k < max_iter);

    if (k == max_iter)
    {
      pointsInSetCount++;
    }
```

### 核心点：

1. 使用 openMP 进行并行计算：`schedule(dynamic)` 动态多线程调度 - 常规操作
2. 双循环使用 `parallel for` 这个不用额外进行操作，但是注意双循环的 index 变量是独立的
3. 循环（线程）内部变量要 **重新声明**，为得是 `private` 这样没有数据的冲突
4. 对于最后面 poinsInSetCount++ ，这个明显是不能冲突的。有两种解决办法：
   1. 使用 `critical` 进行间隔，但是明显效率不好，因为有大量冲突的情况
   2. 使用 `reduction(+: pointsInSetCount)` 函数式编程，直接把多个线程最后的结果进行操作 - **高效**

----

### 注意：

1. 并不需要 **显式** 声明使用的线程数目，`dynamic` 会自动配置好
2. `static` 的效率并没有 `dynamic` 好。

----

原来的 Makefile 文件中使用 `mpicxx` 并不能成功执行，切换后可以执行了。

```cpp
# CXX = mpicxx
CXX = c++
```

