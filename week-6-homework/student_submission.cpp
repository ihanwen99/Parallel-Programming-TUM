//
// Created by Dennis-Florian Herr on 13/06/2022.
//

#include <string>
#include <deque>
#include <future>
#include <thread>
#include <functional>

#include "Utility.h"

#define MEASURE_TIME true
#define NUM_THREAD 32

using namespace std;

struct Problem
{
    Sha1Hash sha1_hash;
    int problemNum;
};

/*
 * TODO@Students: Implement a thread safe queue.
 * Tip: use a condition variable to make threads wait when the queue is empty and there is nothing to pop().
 * https://en.cppreference.com/w/cpp/thread/condition_variable
 */
class ProblemQueue
{
public:
    void push(Problem problem)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            problem_queue.push_back(problem);
        }
        cv.notify_one();
    }

    Problem pop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        while (problem_queue.empty())
        {
            cv.wait(lock);
        }
        Problem p = problem_queue.front();
        problem_queue.pop_front();
        return p;
    }

    bool empty()
    {
        return problem_queue.empty();
    }

private:
    std::deque<Problem> problem_queue;
    std::mutex mutex;
    std::condition_variable cv;
};

ProblemQueue problemQueue;
Sha1Hash *solutionHashes;
// generate numProblems sha1 hashes with leadingZerosProblem leading zero bits
// This method is intentionally compute intense so you can already start working on solving
// problems while more problems are generated
void generateProblem(int seed, int numProblems, int leadingZerosProblem)
{
    srand(seed);

    for (int i = 0; i < numProblems; i++)
    {
        std::string base = std::to_string(rand()) + std::to_string(rand());
        Sha1Hash hash = Utility::sha1(base);
        do
        {
            // we keep hashing ourself until we find the desired amount of leading zeros
            hash = Utility::sha1(hash);
        } while (Utility::count_leading_zero_bits(hash) < leadingZerosProblem);
        problemQueue.push(Problem{hash, i});
    }
}

// This method repeatedly hashes itself until the required amount of leading zero bits is found
Sha1Hash findSolutionHash(Sha1Hash hash, int leadingZerosSolution)
{
    do
    { // we keep hashing ourself until we find the desired amount of leading zeros
        hash = Utility::sha1(hash);
    } while (Utility::count_leading_zero_bits(hash) < leadingZerosSolution);

    return hash;
}

void worker_thread(int leadingZerosSolution)
{
    while (true)
    {
        Problem p = problemQueue.pop();
        if (p.problemNum < 0)
        {
            break;
        }
        solutionHashes[p.problemNum] = findSolutionHash(p.sha1_hash, leadingZerosSolution);
    }
}

int main(int argc, char *argv[])
{
    int leadingZerosProblem;
    int leadingZerosSolution;
    int numProblems;
    // Not interesting for parallelization
    Utility::parse_input(numProblems, leadingZerosProblem, leadingZerosSolution, argc, argv);
    solutionHashes = new Sha1Hash[numProblems];
    unsigned int seed = Utility::readInput();

    /*
     * TODO@Students: Generate the problem in another thread and start already working on solving the problems while the generation continues
     */

    std::thread generatorThread(generateProblem, seed, numProblems, leadingZerosProblem);
    std::thread threads[NUM_THREAD];

    for (int i = 0; i < NUM_THREAD; i++)
    {
        threads[i] = std::thread(worker_thread, leadingZerosSolution);
    }

    // Wait for the problem generation to end
    generatorThread.join();

    for (int i = 0; i < NUM_THREAD; i++)
    {
        problemQueue.push(Problem{Sha1Hash(), -1});
    }

    for (int i = 0; i < NUM_THREAD; i++)
    {
        threads[i].join();
    }
    /*
     * TODO@Students: Create worker threads that parallelize this functionality. Add the synchronization directly to the queue
     */
    while (!problemQueue.empty())
    {
        Problem p = problemQueue.pop();
        solutionHashes[p.problemNum] = findSolutionHash(p.sha1_hash, leadingZerosSolution);
    }

    /*
     * TODO@Students: Make sure all work has finished before calculating the solution
     * Tip: Push a special problem for each thread onto the queue that tells a thread to break and stop working
     */

    Sha1Hash solution;
    // guarantee initial solution hash data is zero
    memset(solution.data, 0, SHA1_BYTES);
    // this doesn't need parallelization. it's neglectibly fast
    for (int i = 0; i < numProblems; i++)
    {
        solution = Utility::sha1(solution, solutionHashes[i]);
    }

    Utility::printHash(solution);
    printf("DONE\n");

    return 0;
}
