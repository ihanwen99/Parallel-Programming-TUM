#include <cstring>
#include <iostream>
#include <chrono>

#include "vv-aes.h"
using namespace std;
using namespace chrono;

/**
 * This function takes the characters stored in the 6x6 message array and substitutes each character for the
 * corresponding replacement as specified in the originalCharacter and substitutedCharacter array.
 * This corresponds to step 2.1 in the VV-AES explanation.
 */

/**
 * 生成查找数组
 * 直接放反转结果
int dict[UNIQUE_CHARACTERS];
for (unsigned int i = 0; i < UNIQUE_CHARACTERS; ++i){
    dict[originalCharacter[i]] = i;
*/
uint8_t reverseOriginalCharacter[]={196, 153, 97, 122, 139, 152, 80, 247, 0, 59, 119, 207, 195, 238, 71, 13, 24, 179, 126, 118, 248, 20, 25, 142, 6, 188, 145, 3, 117, 211, 177, 169, 244, 176, 235, 141, 47, 113, 104, 148, 161, 219, 70, 7, 86, 4, 167, 92, 64, 62, 133, 48, 140, 253, 143, 76, 178, 184, 103, 115, 30, 68, 171, 121, 209, 46, 151, 116, 174, 36, 231, 254, 220, 75, 134, 185, 82, 98, 163, 255, 108, 130, 227, 77, 28, 237, 10, 19, 29, 44, 236, 240, 158, 166, 144, 21, 221, 45, 55, 93, 1, 216, 199, 251, 225, 120, 102, 226, 239, 79, 198, 50, 84, 127, 52, 83, 15, 245, 208, 100, 5, 41, 210, 224, 131, 42, 212, 123, 135, 146, 53, 11, 58, 9, 233, 60, 128, 39, 218, 175, 72, 61, 74, 193, 99, 38, 228, 155, 159, 186, 87, 88, 156, 232, 90, 96, 107, 65, 17, 192, 205, 165, 124, 105, 40, 234, 89, 2, 81, 57, 94, 202, 182, 206, 201, 66, 173, 191, 203, 138, 129, 12, 215, 181, 241, 214, 111, 34, 170, 112, 109, 242, 160, 91, 217, 162, 101, 243, 149, 222, 164, 172, 37, 14, 114, 18, 125, 150, 33, 73, 204, 168, 136, 187, 32, 190, 23, 250, 249, 69, 183, 147, 8, 67, 252, 35, 16, 110, 56, 54, 230, 49, 95, 31, 157, 106, 63, 223, 85, 26, 246, 180, 43, 51, 132, 200, 194, 78, 229, 213, 22, 137, 154, 197, 27, 189};

void substitute_bytes()
{
    // For each byte in the message
    for (int column = 0; column < MESSAGE_BLOCK_SIZE; column++)
    {
        for (int row = 0; row < MESSAGE_BLOCK_SIZE; row++)
        {
            // Search for the byte in the original character list
            // and replace it with corresponding the element in the substituted character list

            // 更新区域
            //for-loop的查找可以优化
            // for (int i = 0; i < UNIQUE_CHARACTERS; i++) {
            //     if (originalCharacter[i] == message[row][column]) {
            //         index = i;
            //     }
            // }

            //修改成直接查表就能找到对应就好了
            //solution - 创建一个表，然后查表

            //把每个位置都换成 替换列表里面的元素
            // message[row][column] = substitutedCharacter[index];
            message[row][column] = substitutedCharacter[reverseOriginalCharacter[message[row][column]]];
        }
    }
}

/*
 * This function shifts (rotates) a row in the message array by one place to the left.
 * @param row The row which to shift.
 */
void shift_row(int row)
{
    // This does a shift (really a rotate) of a row, copying each element to the left
    auto *newRow = (unsigned char *)(malloc(MESSAGE_BLOCK_SIZE));

    //评论：操作已经很直白了
    for (int i = 0; i < MESSAGE_BLOCK_SIZE; ++i)
    {
        newRow[i % MESSAGE_BLOCK_SIZE] = message[row][(i + 1) % MESSAGE_BLOCK_SIZE];
    }


    memcpy(message[row], newRow, MESSAGE_BLOCK_SIZE);
    free(newRow);
}

/*
 * This function shifts each row by the number of places it is meant to be shifted according to the AES specification.
 * Row zero is shifted by zero places. Row one by one, etc.
 * This corresponds to step 2.2 in the VV-AES explanation.
 */
void shift_rows()
{ // 外层的大函数
    // Shift each row, where the row index corresponds to how many columns the data is shifted.
    for (int row = 0; row < MESSAGE_BLOCK_SIZE; ++row)
    {
        for (int shifts = 0; shifts < row; ++shifts)
        {
            shift_row(row);
        }
    }
}

/*
 * This function calculates x^n for polynomial evaluation.
 */
int power(int x, int n)
{
    // Calculates x^n
    if (n == 0)
    {
        return 1;
    }
    return x * power(x, n - 1);
}

/*
 * This function evaluates four different polynomials, one for each row in the column.
 * Each polynomial evaluated is of the form
 * m'[row, column] = c[r][3] m[3][column]^4 + c[r][2] m[2][column]^3 + c[r][1] m[1][column]^2 + c[r][0]m[0][column]^1
 * where m' is the new message value, c[r] is an array of polynomial coefficients for the current result row (each
 * result row gets a different polynomial), and m is the current message value.
 *
 */
void multiply_with_polynomial(int column)
{
    for (int row = 0; row < MESSAGE_BLOCK_SIZE; ++row)
    {
        int result = 0;
        // 这个循环可以优化
        for (int degree = 0; degree < MESSAGE_BLOCK_SIZE; degree++)
        {
            result += polynomialCoefficients[row][degree] * power(message[degree][column], degree + 1);
        }
        message[row][column] = result;
    }
}

/*
 * For each column, mix the values by evaluating them as parameters of multiple polynomials.
 * This corresponds to step 2.3 in the VV-AES explanation.
 */
void mix_columns()
{
    for (int column = 0; column < MESSAGE_BLOCK_SIZE; ++column)
    {
        multiply_with_polynomial(column);
    }
}

/*
 * Add the current key to the message using the XOR operation.
 */

//直接矩阵操作会不会更快？
void add_key()
{
    for (int column = 0; column < MESSAGE_BLOCK_SIZE; column++)
    {
        for (int row = 0; row < MESSAGE_BLOCK_SIZE; ++row)
        {
            // ^ == XOR
            message[row][column] = message[row][column] ^ key[row][column];
        }
    }
}

/*
 * Your main encryption routine.
 */
int main(){
    // Receive the problem from the system.
    readInput();
    // cout << "Computing is Start" << endl;
    // auto start = system_clock::now();

    // For extra security (and because Varys wasn't able to find enough test messages to keep you occupied) each message
    // is put through VV-AES lots of times. If we can't stop the adverse Maesters from decrypting our highly secure
    // encryption scheme, we can at least slow them down.
    for (int i = 0; i < NUM_ITERATIONS; i++)
    {
        // For each message, we use a predetermined key (e.g. the password). In our case, its just pseudo random.
        set_next_key();

        // First, we add the key to the message once:
        add_key();

        // We do 9+1 rounds for 128 bit keys
        for (int round = 0; round < NUM_ROUNDS; round++)
        {
            // In each round, we use a different key derived from the original (refer to the key schedule).
            set_next_key();

            // These are the four steps described in the slides.
            // 那么也就是我们的优化目标
            substitute_bytes();
            shift_rows();
            mix_columns();
            add_key();
        }
        // Final round
        substitute_bytes();
        shift_rows();
        add_key();
    }

    // Submit our solution back to the system.
    writeOutput();
    // auto end = system_clock::now();
    // auto duration = duration_cast<microseconds>(end - start);
    // cout << "All Process is : " << double(duration.count()) * microseconds::period::num / microseconds::period::den << "s" << endl;
    return 0;
}

