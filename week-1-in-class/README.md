# Week1 - Homework

预先计算出一个反转数组，这样可以避免实际执行的时候多次查找浪费时间。

```cpp
uint8_t reverseOriginalCharacter[];
  
void substitute_bytes()
{
    // For each byte in the message
    for (int column = 0; column < MESSAGE_BLOCK_SIZE; column++)
    {
        for (int row = 0; row < MESSAGE_BLOCK_SIZE; row++)
        {
            message[row][column] = substitutedCharacter[reverseOriginalCharacter[message[row][column]]];
        }
    }
}
```

把代码融合在一起实现进一步的加速

```cpp
void shift_rows()
{
    // Shift each row, where the row index corresponds to how many columns the data is shifted.
    char(*newMessage)[MESSAGE_BLOCK_SIZE] = (char(*)[MESSAGE_BLOCK_SIZE])malloc(sizeof(char) * MESSAGE_BLOCK_SIZE * MESSAGE_BLOCK_SIZE);
    for (int row = 1; row < MESSAGE_BLOCK_SIZE; ++row)
    {
        for (int i = 0; i < MESSAGE_BLOCK_SIZE; ++i)
        {
            newMessage[row][i % MESSAGE_BLOCK_SIZE] = message[row][(i + row) % MESSAGE_BLOCK_SIZE];
        }
    }

    for (int row = 1; row < MESSAGE_BLOCK_SIZE; ++row)
    {
        for (int column = 0; column < MESSAGE_BLOCK_SIZE; ++column)
        {
            message[row][column] = newMessage[row][column];
        }
    }
}
```

