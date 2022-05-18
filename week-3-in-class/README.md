# In-class exercise 3: Help to parallelize K-NN
There were no new time breaches this week.
But we received a corporate order which will help us pay out salaries and save time for future time breaches.
A company called Bambridge Neolitica has a Facebook test app, which predicts a Hogwarts House for the user based on their answers.
They are making this prediction using the K-Nearest-Neighbors algorithm. You have to help us optimize/parallelize it.
We have a set of data points which is used for the predictions and a set of input points for which we have to make these predictions. All points are assumed to be in 2D. Each point in the data point cloud has a class label (for ex. Gryffindor - 0, Slytherin - 1). The algorithm is executed as follows:

1. For a point P in the input point cloud, we find the k nearest points in the data point cloud.
2. Count how many times each class appears among the k nearest neighbors. Assign the class label which appears the most to point P.
3. Repeat step 1 and step 2 for every point in the input point cloud.
4. Count the number of points in each class for the input point cloud and print.

## Your tasks
1. Add OpenMP pragmas to **student_submission.cpp** to achieve parallelism.
2. Submit your code written in **student_submission.cpp** to [https://parprog.caps.in.tum.de/](https://parprog.caps.in.tum.de/).
3. Make sure you reach a speedup of 15.

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

# Week3 - In class

已将 `brew install` 的 `gcc` 添加至环境变量。现在已删除。之后切换到 Ubuntu 虚拟机完成作业。

---

```cpp
void predict(double input_data[][2], int *predicted_class, int *each_class_num)
{
    int k_nearest_neighbors[NUM_PREDICTIONS][K];

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < NUM_PREDICTIONS; i++)
    {
        get_k_nearest_neighbors(input_data[i], k_nearest_neighbors[i]);
        get_class_from_neighbors(k_nearest_neighbors[i], &predicted_class[i]);
#pragma omp atomic
        each_class_num[predicted_class[i]]++;
    }
}
```

最快的方式就是使用 `schedule(dynamic)`

然后要关注可能会被多个线程所修改的变量，使用 `atomic` 进行保护

---

### Makefile

Makefile里主要包含了五个东西：变量定义、显式规则、隐晦规则、文件指示和注释。

1、变量的定义。在Makefile中我们要定义一系列的变量，变量一般都是字符串，这个有点像C语言中的宏，当Makefile被执行时，其中的变量都会被扩展到相应的引用位置上。

2、显式规则。显式规则说明了，如何生成一个或多的的目标文件。这是由Makefile的书写者明显指出，要生成的文件，文件的依赖文件，生成的命令。 刚才写的疑似shell脚本的Makefile全部都是显示规则。

3、隐晦规则。由于我们的make有自动推导的功能，所以隐晦的规则可以让我们比较粗糙地简略地书写Makefile，这是由make所支持的。

4、文件指示。其包括了三个部分，一个是在一个Makefile中引用另一个Makefile，就像C语言中的include一样。

5、注释。Makefile中只有行注释，和UNIX的Shell脚本一样，其注释是用“#”字符，这个就像C/C++中的“//”一样。如果你要在你的Makefile中使用“#”字符。

**常用的预定义变量**

$* 　　不包含扩展名的目标文件名称。

$+ 　　所有的依赖文件，以空格分开，并以出现的先后为序，可能包含重复的依赖文件。

$< 　　第一个依赖文件的名称。

$?		  所有的依赖文件，以空格分开，这些依赖文件的修改日期比目标的创建日期晚。

$@		目标的完整名称。

$^ 		所有的依赖文件，以空格分开，不包含重复的依赖文件。

$%		如果目标是归档成员，则该变量表示目标的归档成员名称。

----

### Clang 和 GCC 的区别

**GCC（GNU Compiler Collection）**是一套有GNU开发的编程语言编译器，以GPL及LGPL许可证所发行的自由软件。GCC原名为GNU C 语言编译器，它原本只能处理C语言，随着技术的发展，GCC很快地得到扩展，变得可以处理C++，之后可以处理的语言扩展到Fortran、Pascal、Objective-C、Java等。

**LLVM（Low Level Virtual Machine**），即底层虚拟机。它是一个由C++编写而成的编译器基础框架，利用虚拟技术创造出编译时期、链接时期、运行时期以及“闲置时期”的最优化框架。从宏观上来讲，LLVM不仅仅是一个编译器或者虚拟机，它是一个众多编译器工具及低级工具技术的统称，它包含了一个前端、优化器、后端以及众多的函数库和模板。从微观上来讲，可以把它看做后端编译器，用来生成目标代码，前端编译器为**Clang**。

Xcode5版本之前，编译器默认使用的是GCC，从Xcode5之后编译器默认使用LLVM。

**CLang**是一个由C++编写的编译器前端，能够编译C/C++/Objective等高级语言，属于LLVM的一部分，发布于BSD（自由软件中使用最广发的许可证之一）许可证下，**其目的就是为了超越GCC**。经过测试证明，Clang编译Objective-C代码的速度为GCC的3倍左右，同时它还能针对用户发生的编译错误准确地给出建议。

**Clang和GCC的主要区别如下所示。**

1. Clang比GCC编译用的时间更短，包括预处理、语法分析、解析、语义分析、抽象语法树生成的时间。
2. Clang比GCC的内存占用更小。
3. Clang生成的中间产物比GCC更小。
4. Clang的错误提示比GCC更加友好。
5. Clang有静态分析，GCC没有。
6. Clang使用BSD许可证，GCC使用GPL许可证。
7. Clang从一开始就被设计为一个API，允许它被源代码分析工具和IDE集成。GCC被构建成一个单一的静态编译器，这使得它非常难以被作为API并集成到其他工具中。
8. GCC比Clang支持更多的语言，例如Java。
9. GCC比Clang支持更多的平台。
10. GCC比Clang更流行。

