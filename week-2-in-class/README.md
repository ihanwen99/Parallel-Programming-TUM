# In-class exercise 2: Help the starship detect damage
We have received an urgent request from our colleagues from Proxima B. Their Timekeepers team is understaffed and they need our help to save some time. Thus you have to optimize one of their programs. You are provided with a code snippet which is used by starships in the Proxima Centauri system to avoid being hit and detect damage from meteorites. Proxima B has multicore processors installed on their starships, so you are welcome to use threads to achieve maximum speedup.

## Your tasks
1. Fill in the code in **student_submission.cpp** to optimize the damage detection process.
2. Submit your code written in **student_submission.cpp** to [https://parprog.caps.in.tum.de/](https://parprog.caps.in.tum.de/).
3. Make sure you reach a speedup of 12.

Remember - you only have 10 minutes.

## Technical details
The files Utility.h and Makefile were already sent to the server. You can only modify student_submission.cpp (in the beginning identical to sequential_implementation.cpp). As soon as you have optimized your code, upload ONLY student_submission.cpp to the server and check if your message was sent successfully.

Good luck! 

## How to run the code

```bash
make
# run sequential implementation
./sequential_implementation
# run your solution
./student_implementation
```

# Week2 - In class

1. 创建多线程

```cpp
// TODO: uncomment once you added the correct headers
#define THREAD_NUM 32
std::thread threads[THREAD_NUM];

for (int thread_id = 0; thread_id < THREAD_NUM; thread_id++)
{
  int each = width / THREAD_NUM;
  int low = each * thread_id;
  threads[thread_id] = std::thread(working_thread, low, low + each);
}

for (int thread_id = 0; thread_id < THREAD_NUM; thread_id++)
{
  threads[thread_id].join();
}
```

2. 使用锁保护 多个线程都能操作的变量

```cpp
// ######################## TODO: Copy task id from the global variable and increment it 
// ######################## DO NOT FORGET TO LOCK AND UNLOCK! ########################
std::mutex mutex;

mutex.lock();
local_task_id=task_id++;
mutex.unlock();
// ######################## TODO END ########################
```

