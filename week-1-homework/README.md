# Week1 - Homework

预先计算出一个反转数组，这样可以避免实际执行的时候多次查找浪费时间。

```cpp
int dict[UNIQUE_CHARACTERS];
for (unsigned int i = 0; i < UNIQUE_CHARACTERS; ++i){
    dict[originalCharacter[i]] = i;
}
```

**待补充...**