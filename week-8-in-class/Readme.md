# In-class exercise 8: Help parallelize wave simulation
This time, we are going to face the wave simulation again! Remember last time that Jack Sparrow has successfully found his way to his treasure on a stromy sea? Since we want to extend the parallelization to multiple computers with distributed memory, we need the help of MPI library! This time, let's work on a simpler wave simulation problem, but with MPI. Let's see how much time you can save for us!


## Your tasks
1. Add Intel Intrinsics vector operations to **student_submission.cpp** to achieve parallelism.
2. Submit your code written in **student_submission.cpp** to [https://parprog.caps.in.tum.de/](https://parprog.caps.in.tum.de/).
3. Make sure you reach a speedup of 2.

Remember - you only have 10 minutes.

## Technical details
The files Utility.h and Makefile were already sent to the server. You can only modify student_submission.cpp (there are multiple TODOs comment in the code, finish them). As soon as you have optimized your code, upload ONLY student_submission.cpp to the server and check if your message was sent successfully.

Good luck! 

## How to run the code

```bash
make
# run sequential implementation
./sequential_implementation
# run your solution
mpirun ./student_implementation
```

---

### Core Implementation

程序的基本框架：初始化信息+最终结束

```cpp
#include <mpi.h>

// TODO: initialize the MPI process
MPI_Init(&argc, &argv);
// define the rank and size variable
int rank, size;

// TODO: get the process rank from MPI
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
// TODO: get the total processes size from MPI
MPI_Comm_size(MPI_COMM_WORLD, &size);

// pass the wave map to all other processes
MPI_Bcast(old_wave_map, MAP_HEIGHT * MAP_WIDTH, MPI_FLOAT, 0, MPI_COMM_WORLD);

// TODO: finalize the MPI process
MPI_Finalize();
```

模拟 `if` 的情况，补充上 `else` 的情况。

需要注意：

1. rank == 0 的情况
2.  注意 `api` 的实现以及对应的坐标 `id`

```cpp
// the number of rows calculated by a process
int proc_rows = MAP_HEIGHT / size;
// the starting row in this process
int st_row = rank * proc_rows;
// the end row(not included) in this process
int end_row = (rank + 1) * proc_rows;

for (int i = 0; i < ITER_NUM; i++)
{
  // communicate the ghost layers before update
  if (rank == 0)
  {
    // send lowest row in block to process with rank 1
    MPI_Send(old_wave_map + (end_row - 1) * MAP_WIDTH, MAP_WIDTH, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
    // receive upper ghost layer from process with rank size-1
    MPI_Recv(old_wave_map + (MAP_HEIGHT - 1) * MAP_WIDTH, MAP_WIDTH, MPI_FLOAT, size - 1, 0, MPI_COMM_WORLD, nullptr);
    // send uppermost row in block to process with rank size-1
    MPI_Send(old_wave_map + st_row * MAP_WIDTH, MAP_WIDTH, MPI_FLOAT, size - 1, 1, MPI_COMM_WORLD);
    // receive lower ghost layer from process with rank 1
    MPI_Recv(old_wave_map + end_row * MAP_WIDTH, MAP_WIDTH, MPI_FLOAT, 1, 1, MPI_COMM_WORLD, nullptr);
  }
  else
  {
    /*
     * TODO: use four lines of codes to finish the following receive and send process for rank other than 0
     *       pay attention to the order of the send and receive.
    */

    // TODO: receive upper ghost layer from process with rank rank-1
    MPI_Recv(old_wave_map + (st_row - 1) * MAP_WIDTH, MAP_WIDTH, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, nullptr);
    // TODO: send lowest row in block to the process with rank (rank+1)%size
    MPI_Send(old_wave_map + (end_row - 1) * MAP_WIDTH, MAP_WIDTH, MPI_FLOAT, (rank + 1) % size, 0, MPI_COMM_WORLD);
    // TODO: receive lower ghost layer from process with rank (rank+1)%size
    MPI_Recv(old_wave_map + (end_row % MAP_HEIGHT) * MAP_WIDTH, MAP_WIDTH, MPI_FLOAT, (rank + 1) % size, 1, MPI_COMM_WORLD, nullptr);
    // TODO: send uppermost row in block to the process with rank rank-1
    MPI_Send(old_wave_map + st_row * MAP_WIDTH, MAP_WIDTH, MPI_FLOAT, rank - 1, 1, MPI_COMM_WORLD);
  }
}
```

