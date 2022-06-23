#include "dgemm.h"
#include <cstdio>
#include <cstdlib>
#include <immintrin.h>

void dgemm_hanwen(float alpha, const float *a, const float *b, float beta, float *c)
{
    for (size_t i = 0; i < MATRIX_SIZE; i++)
    {
        for (size_t j = 0; j < MATRIX_SIZE; j++)
        {
            c[i * MATRIX_SIZE + j] *= beta;

            __m256 partial_sum = _mm256_setzero_ps();
            float partial_sum_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            size_t k = 0;
            for (; k + 8 <= MATRIX_SIZE; k += 8)
            {
                __m256 aa = _mm256_loadu_ps(a + i * MATRIX_SIZE + k);
                __m256 bb = _mm256_loadu_ps(b + j * MATRIX_SIZE + k);

                __m256 tmp = _mm256_mul_ps(aa, bb);
                partial_sum = _mm256_add_ps(partial_sum, tmp);
            }
            _mm256_store_ps(partial_sum_array, partial_sum);

            float sum = 0;
            for (size_t i = 0; i < 8; i++)
            {
                sum += partial_sum_array[i];
            }

            for (; k < MATRIX_SIZE; k++)
            {
                sum += a[i * MATRIX_SIZE + k] * b[j * MATRIX_SIZE + k];
            }

            c[i * MATRIX_SIZE + j] += alpha * sum;
        }
    }
}

int main(int, char **)
{
    float alpha, beta;
    int mem_size = MATRIX_SIZE * MATRIX_SIZE * sizeof(float);
    auto a = (float *)malloc(mem_size);
    auto b = (float *)malloc(mem_size);
    auto c = (float *)malloc(mem_size);
    if (nullptr == a || nullptr == b || nullptr == c)
    {
        printf("Memory allocation failed\n");
        if (nullptr != a)
            free(a);
        if (nullptr != b)
            free(b);
        if (nullptr != c)
            free(c);
        return 0;
    }

    generateProblemFromInput(alpha, a, b, beta, c);

    std::cerr << "Launching dgemm step." << std::endl;

    dgemm_hanwen(alpha, a, b, beta, c);

    outputSolution(c);

    free(a);
    free(b);
    free(c);
    return 0;
}
