# Week8 - Homework
使用 `MPI` 进行并行计算 。

### 文件：

- sequential_implementation: Base Code
- student_submission: My Implementation - Not So Good
- tudent_submission_friend: Team Implementation ✅

### 思路：

main function

```C++
for (int iteration = 0; iteration < NUM_SIMULATION_STEPS; ++iteration)
    {
        copy_edges(*problemData->readGrid, rank, size, st_row, end_row);
        intermediate[iteration] = evolve(*problemData, st_row, end_row);

        problemData->swapGrids();

        int inter_reduce = 0;
        MPI_Reduce(intermediate + iteration, &inter_reduce, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (rank == 0) // Store For Output
        {
            intermediate[iteration] = inter_reduce;
            if (iteration % SOLUTION_REPORT_INTERVAL == 0 && iteration != 0)
            {
                std::cout << "Iteration " << iteration << ": " << intermediate[iteration - 1] << " cells alive." << std::endl;
            }
        }
    }
```

Update - copy_edges

```cpp
void copy_edges(bool (&grid)[GRID_SIZE][GRID_SIZE], int rank, int size, int st_row, int end_row)
{
    for (int i = st_row; i < end_row; i++)
    {
        if (i == 0 || i == (GRID_SIZE - 1))
        {
            continue;
        }
        // join rows together
        grid[i][0] = grid[i][GRID_SIZE - 2];
        grid[i][GRID_SIZE - 1] = grid[i][1];
    }

    if (rank == 0)
    {
        bool buf_send_top[GRID_SIZE];
        for (int j = 0; j < GRID_SIZE; ++j)
        {
            buf_send_top[j] = grid[1][j];
        }
        buf_send_top[0] = grid[1][GRID_SIZE - 2];
        buf_send_top[GRID_SIZE - 1] = grid[1][1];

        MPI_Send(&grid[end_row - 1][0], GRID_SIZE, MPI_CXX_BOOL, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&grid[end_row][0], GRID_SIZE, MPI_CXX_BOOL, 1, 1, MPI_COMM_WORLD, nullptr);
        MPI_Send(&buf_send_top, GRID_SIZE, MPI_CXX_BOOL, size - 1, 2, MPI_COMM_WORLD);
        MPI_Recv(&grid[0][0], GRID_SIZE, MPI_CXX_BOOL, size - 1, 3, MPI_COMM_WORLD, nullptr);
    }
    else if (rank == size - 1)
    {
        bool buf_send_down[GRID_SIZE];
        for (int j = 0; j < GRID_SIZE; ++j)
        {
            buf_send_down[j] = grid[GRID_SIZE - 2][j];
        }

        buf_send_down[0] = grid[GRID_SIZE - 2][GRID_SIZE - 2];
        buf_send_down[GRID_SIZE - 1] = grid[GRID_SIZE - 2][1];

        MPI_Recv(&grid[(st_row)-1][0], GRID_SIZE, MPI_CXX_BOOL, size - 2, 0, MPI_COMM_WORLD, nullptr);
        MPI_Send(&grid[st_row][0], GRID_SIZE, MPI_CXX_BOOL, size - 2, 1, MPI_COMM_WORLD);
        MPI_Recv(&grid[GRID_SIZE - 1][0], GRID_SIZE, MPI_CXX_BOOL, 0, 2, MPI_COMM_WORLD, nullptr);
        MPI_Send(&buf_send_down, GRID_SIZE, MPI_CXX_BOOL, 0, 3, MPI_COMM_WORLD);
    }
    else // Inside Nodes - Attention the Order!
    {
        MPI_Send(&grid[end_row - 1][0], GRID_SIZE, MPI_CXX_BOOL, rank + 1, 0, MPI_COMM_WORLD);
        MPI_Send(&grid[st_row][0], GRID_SIZE, MPI_CXX_BOOL, rank - 1, 1, MPI_COMM_WORLD);
        MPI_Recv(&grid[end_row][0], GRID_SIZE, MPI_CXX_BOOL, rank + 1, 1, MPI_COMM_WORLD, nullptr);
        MPI_Recv(&grid[(st_row - 1)][0], GRID_SIZE, MPI_CXX_BOOL, rank - 1, 0, MPI_COMM_WORLD, nullptr);
    }
}
```

Update - evolve

```cpp
/*
  Apply the game of life rules on a Torus --> grid contains shadow rows and columns
  to simplify application of rules i.e. grid actually ranges from grid [ 1.. height - 2 ][ 1 .. width - 2]
*/
int evolve(ProblemData &problemData, int st_row, int end_row)
{
    auto &grid = *problemData.readGrid;
    auto &writeGrid = *problemData.writeGrid;
    int survivor = 0;
    // For each cell
    for (int i = st_row; i < end_row; i++)
    {
        for (int j = 1; j < GRID_SIZE - 1; j++)
        {
            // FIXME  || i == GRID_SIZE
            if (i == 0 || i == (GRID_SIZE - 1))
            {
                continue;
            }
            // Calculate the number of neighbors
            int sum = grid[i - 1][j - 1] + grid[i - 1][j] + grid[i - 1][j + 1] +
                      grid[i][j - 1] + grid[i][j + 1] +
                      grid[i + 1][j - 1] + grid[i + 1][j] + grid[i + 1][j + 1];

            if (!grid[i][j])
            {
                // If a cell is dead, it can start living by reproduction or stay dead
                if (sum == 3)
                {
                    // reproduction
                    writeGrid[i][j] = true;
                }
                else
                {
                    writeGrid[i][j] = false;
                }
            }
            else
            {
                // If a cell is alive, it can stay alive or die through under/overpopulation
                if (sum == 2 || sum == 3)
                {
                    // stays alive
                    writeGrid[i][j] = true;
                }
                else
                {
                    // dies due to under or overpopulation
                    writeGrid[i][j] = false;
                }
            }

            if (writeGrid[i][j] == 1)
            {
                survivor += 1;
            }
        }
    }
    return survivor;
}
```

Update - Utility Functions

```cpp
std::minstd_rand randomEngine_new;
uint_fast32_t cachedValue_new;
uint_fast32_t bitMask_new = 0;

inline bool generateBit_new()
{
    if (!bitMask_new)
    {
        cachedValue_new = randomEngine_new();
        bitMask_new = 1;
    }
    bool value = cachedValue_new & bitMask_new;
    bitMask_new = bitMask_new << 1;
    return value;
}

void seedGenerator_new(unsigned long long seed)
{
    randomEngine_new = std::minstd_rand(seed);
}

// New Design
// Generate Data Here and Propagate to Others
void readProblemFromInput(ProblemData &data, int rank)
{
    auto &grid = *data.readGrid;
    if (rank == 0) // Same as Original One, Only change to "rank==0"
    {
        unsigned int seed = 0;
        std::cout << "READY" << std::endl;
        std::cin >> seed;

        std::cout << "Using seed " << seed << std::endl;
        if (seed == 0)
        {
            std::cout << "Warning: default value 0 used as seed." << std::endl;
        }

        // "random" numbers
        seedGenerator_new(seed);

        for (int i = 0; i < GRID_SIZE * GRID_SIZE; i += 1)
        {
            *(grid[0] + i) = generateBit_new();
        }
    }

    MPI_Bcast(grid, (GRID_SIZE) * (GRID_SIZE), MPI_CXX_BOOL, 0, MPI_COMM_WORLD);

    if (rank == 2)
    {
        int count_1 = 0;
        for (int i = 1; i < GRID_SIZE - 1; i++)
        {
            for (int j = 1; j < GRID_SIZE - 1; j++)
            {
                if (grid[i][j] == 1)
                {
                    count_1++;
                }
            }
        }
        std::cout << "Iteration 0"
                  << ": " << count_1 << " cells alive." << std::endl;
    }
}
```

### 需要注意的细节问题：

1. MPI 实现框架
2. MPI Send Recv 需要注意顺序和对应关系，避免死锁；并且对于不同的 rank 要有不同的操作
3. 使用 buffer 重新组织数据
