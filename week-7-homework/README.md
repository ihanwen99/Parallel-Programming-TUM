# Week7 - Homework

使用 `Intel Intrinsics` 并行化矩阵乘法的操作。

### 思路：

1. `_mm256_load_ps` & `_mm256_loadu_ps` ：`u` 代表对齐的含义

   ```cpp
   __m256 aa = _mm256_loadu_ps(a + i * MATRIX_SIZE + k);
   __m256 bb = _mm256_loadu_ps(b + j * MATRIX_SIZE + k);
   ```

2. `_mm256_mul_ps` ：AVX 的矩阵乘法

   ```cpp
	__m256 tmp = _mm256_mul_ps(aa, bb);
	```
	
3. `_mm256_add_ps` ：AVX 的加法	

	```cpp
	__m256 partial_sum = _mm256_setzero_ps();
	partial_sum = _mm256_add_ps(partial_sum, tmp);
	```

4. `_mm256_store_ps` ：再把结果放回去

   ```cpp
   float partial_sum_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   _mm256_store_ps(partial_sum_array, partial_sum);
   ```

5. 因为放回去是 8 个 float 变量，所以就额外把他们加起来

   ```cpp
   float sum = 0;
   for (int i = 0; i < 8; i++)
   {
     sum += partial_sum_array[i];
   }
   ```

6. 操作的时候需要注意边缘：不能整除的部分进行单独的操作

   ```cpp
   c[i * MATRIX_SIZE + j] *= beta;
   
   // intrinsics
   __m256 partial_sum = _mm256_setzero_ps();
   float partial_sum_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
   
   // 使用 MAX_SIZE=2048 验证没有问题
   // 现在 MAX_SIZE=2017 不能整除
   int k = 0;
   for (; k + 8 <= MATRIX_SIZE; k += 8)
   {
     // mm256 loadu - 这种计算就相当快！
     // 测试发现必须 loadu， load 还是报错
     __m256 aa = _mm256_loadu_ps(a + i * MATRIX_SIZE + k);
     __m256 bb = _mm256_loadu_ps(b + j * MATRIX_SIZE + k);
   
     __m256 tmp = _mm256_mul_ps(aa, bb);
     partial_sum = _mm256_add_ps(partial_sum, tmp);
   }
   _mm256_store_ps(partial_sum_array, partial_sum);
   
   float sum = 0;
   for (int i = 0; i < 8; i++)
   {
     sum += partial_sum_array[i];
   }
   
   for (; k < MATRIX_SIZE; k++)
   {
     sum += a[i * MATRIX_SIZE + k] * b[j * MATRIX_SIZE + k];
   }
   
   c[i * MATRIX_SIZE + j] += alpha * sum;
   ```

### 发现的细节问题：

1. 变量命名：

   ```cpp
   __m256 aa = _mm256_maskload_ps(a + i * mat_size + k);
   ```

   最开始没有注意到变量名的问题，`aa` 和 `a` 重叠了，导致出现以下报错：

   `error: conversion of scalar ‘size_t’ {aka ‘long unsigned int’} to vector ‘__m256’ {aka ‘__vector(8) float’} involves truncation`

2. 32位对齐：

   用`_m256_load_*`把数据从内存加载到 `__mm256` 变量里，要求内存里的数据必须按32位对齐，否则会报段错误。用 `_m256_loadu_*` 来加载不一定对齐的数据。

   **修正以上问题前：**编译不报错，实际执行的时候出现 `Segmentation fault`

   **修正以上问题后：**执行不报错，但是结果不对

3. MATRIX_SIZE=2017

   发现 `MATRIX_SIZE=2017` ，不能整除的情况，那么读取的最后部分元素不是实际矩阵的元素。 
   
   如果将  `MATRIX_SIZE` 改为 1024，结果相同，确认是**不能整除**的问题。 

### 参考资料

[Intel 内部指令 --- AVX和AVX2学习笔记](https://blog.csdn.net/just_sort/article/details/94393506)

[Intel Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#techs=AVX)
