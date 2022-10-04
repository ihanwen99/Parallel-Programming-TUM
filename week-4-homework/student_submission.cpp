#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <getopt.h>

#include "a-star-navigator.h"
#include "VideoOutput.h"
#include "Utility.h"

void simulate_waves(ProblemData &problemData)
{
    auto &islandMap = problemData.islandMap;
    float(&secondLastWaveIntensity)[MAP_SIZE][MAP_SIZE] = *problemData.secondLastWaveIntensity;
    float(&lastWaveIntensity)[MAP_SIZE][MAP_SIZE] = *problemData.lastWaveIntensity;
    float(&currentWaveIntensity)[MAP_SIZE][MAP_SIZE] = *problemData.currentWaveIntensity;

#pragma omp parallel for schedule(dynamic)
    for (int x = 1; x < MAP_SIZE - 1; ++x)
    {
        for (int y = 1; y < MAP_SIZE - 1; ++y)
        {

            // Simulate some waves
            // The acceleration is the relative difference between the current point and the last.
            float acceleration = ATTACK_FACTOR * (lastWaveIntensity[x][y - 1] + lastWaveIntensity[x - 1][y] + lastWaveIntensity[x + 1][y] + lastWaveIntensity[x][y + 1]) - (4 * ATTACK_FACTOR - 1) * lastWaveIntensity[x][y] - secondLastWaveIntensity[x][y];

            float energyPreserved = std::clamp(
                ENERGY_PRESERVATION_FACTOR * (LAND_THRESHOLD - 0.1f * islandMap[x][y]), 0.0f, 1.0f);

            // There aren't any waves on land.
            if (islandMap[x][y] >= LAND_THRESHOLD)
            {
                currentWaveIntensity[x][y] = 0.0f;
            }
            else
            {
                currentWaveIntensity[x][y] =
                    std::clamp(lastWaveIntensity[x][y] + acceleration * energyPreserved, 0.0f, 1.0f);
            }
        }
    }
}

// Since all pirates like navigating by the stars, Captain Jack's favorite pathfinding algorithm is called A*.
// Unfortunately, sometimes you just have to make do with what you have. So here we use a search algorithm that searches
// the entire domain every time step and calculates all possible ship positions.
bool findPathWithExhaustiveSearch(ProblemData &problemData, int timestep,
                                  std::vector<Position2D> &pathOutput)
{
    auto &start = problemData.shipOrigin;
    auto &portRoyal = problemData.portRoyal;
    auto &islandMap = problemData.islandMap;
    auto &currentWaveIntensity = *problemData.currentWaveIntensity;

    // int numPossiblePositions = 0;

    bool(&currentShipPositions)[MAP_SIZE][MAP_SIZE] = *problemData.currentShipPositions;
    bool(&previousShipPositions)[MAP_SIZE][MAP_SIZE] = *problemData.previousShipPositions;

    // We could always have been at the start in the previous frame since we get to choose when we start our journey.
    previousShipPositions[start.x][start.y] = true;
    int flag = 0;

// Ensure that our new buffer is set to zero. We need to ensure this because we are reusing previously used buffers.
#pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < MAP_SIZE; ++x)
    {
        for (int y = 0; y < MAP_SIZE; ++y)
        {
            currentShipPositions[x][y] = false;
        }
    }

    // Do the actual path finding.
#pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < MAP_SIZE; ++x)
    {
        for (int y = 0; y < MAP_SIZE; ++y)
        {
            // If there is no possibility to reach this position then we don't need to process it.
            if (!previousShipPositions[x][y])
            {
                continue;
            }
            Position2D previousPosition(x, y);

            // The Jolly Mon (Jack's ship) is not very versatile. It can only move along the four cardinal directions by one
            // square each and along their diagonals. Alternatively, it can just try to stay where it is.
            // If we are not yet done then we have to take a look at our neighbors.
            for (Position2D &neighbor : neighbours)
            {
                // Get the neighboring position we are examining. It is one step further in time since we have to move
                // there first.
                Position2D neighborPosition = previousPosition + neighbor;

                // If position is out of bounds, skip it
                if (neighborPosition.x < 0 || neighborPosition.y < 0 || neighborPosition.x >= MAP_SIZE || neighborPosition.y >= MAP_SIZE)
                {
                    continue;
                }

                // If this position is already marked, skip it
                if (currentShipPositions[neighborPosition.x][neighborPosition.y])
                {
                    continue;
                }

                // If we can't sail to this position because it is either on land or because the wave height is too
                // great for the Jolly Mon to handle, skip it
                if (islandMap[neighborPosition.x][neighborPosition.y] >= LAND_THRESHOLD ||
                    currentWaveIntensity[neighborPosition.x][neighborPosition.y] >= SHIP_THRESHOLD)
                {
                    continue;
                }

                // If we reach Port Royal, we win.
                if (neighborPosition == portRoyal)
                {
                    flag = 1;
                }

                currentShipPositions[neighborPosition.x][neighborPosition.y] = true;
            }
        }
    }
    if (flag == 1)
        return true;

#pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < MAP_SIZE; ++x)
    {
        for (int y = 0; y < MAP_SIZE; ++y)
        {
            // If there is no possibility to reach this position then we don't need to process it.
            if (!previousShipPositions[x][y])
            {
                continue;
            }
            Position2D previousPosition(x, y);

            // The Jolly Mon (Jack's ship) is not very versatile. It can only move along the four cardinal directions by one
            // square each and along their diagonals. Alternatively, it can just try to stay where it is.
            // If we are not yet done then we have to take a look at our neighbors.
            for (Position2D &neighbor : neighbours)
            {
                // Get the neighboring position we are examining. It is one step further in time since we have to move
                // there first.
                Position2D neighborPosition = previousPosition + neighbor;

                // If position is out of bounds, skip it
                if (neighborPosition.x < 0 || neighborPosition.y < 0 || neighborPosition.x >= MAP_SIZE || neighborPosition.y >= MAP_SIZE)
                {
                    continue;
                }

                // If this position is already marked, skip it
                if (currentShipPositions[neighborPosition.x][neighborPosition.y])
                {
                    continue;
                }

                // If we can't sail to this position because it is either on land or because the wave height is too
                // great for the Jolly Mon to handle, skip it
                if (islandMap[neighborPosition.x][neighborPosition.y] >= LAND_THRESHOLD ||
                    currentWaveIntensity[neighborPosition.x][neighborPosition.y] >= SHIP_THRESHOLD)
                {
                    continue;
                }

                currentShipPositions[neighborPosition.x][neighborPosition.y] = true;
                // flag += add;
                std::cout << flag << std::endl;
                // numPossiblePositions++;
            }
        }
    }
    // This is not strictly required but can be used to track how much additional memory our path traceback structures
    // are using.
    problemData.numPredecessors += problemData.nodePredecessors[timestep].size();

    return false;
}

// Your main simulation routine.
int main(int argc, char *argv[])
{
    bool outputVisualization = false;
    bool constructPathForVisualization = false;
    int numProblems = 1;
    int option;

    // Not interesting for parallelization
    Utility::parse_input(outputVisualization, constructPathForVisualization, numProblems, option, argc, argv);

    // Fetch the seed from our container host used to generate the problem. This starts the timer.
    unsigned int seed = 0;
    std::cout << "READY" << std::endl;
    std::cin >> seed;

    // Note that on the submission server, we are solving "numProblems" problems

    //#pragma parallel for schedule(dynamic)
    for (int problem = 0; problem < numProblems; ++problem)
    {
        //
        // Receive the problem from the system.

        auto *problemData = new ProblemData();
        // problemData->outputVisualization = false;
        // problemData->constructPathForVisualization = false;

        Utility::generateProblem((seed + problem * JUMP_SIZE) & INT_LIM, *problemData);

        // std::cerr << "Searching from ship position (" << problemData->shipOrigin.x << ", " << problemData->shipOrigin.y
        //           << ") to Port Royal (" << problemData->portRoyal.x << ", " << problemData->portRoyal.y << ")."<< std::endl;

        int pathLength = -1;
        std::vector<Position2D> path;

        for (int t = 2; t < TIME_STEPS; t++)
        {
            // First simulate all cycles of the storm
            simulate_waves(*problemData);

            // Help captain Sparrow navigate the waves
            if (findPathWithExhaustiveSearch(*problemData, t, path))
            {
                // The length of the path is one shorter than the time step because the first frame is not part of the
                // pathfinding, and the second frame is always the start position.
                pathLength = t - 1;
                break;
            }

            // Rotates the buffers, recycling no longer needed data buffers for writing new data.
            problemData->flipSearchBuffers();
            problemData->flipWaveBuffers();
        }
        // Submit our solution back to the system.
        Utility::writeOutput(pathLength);

        delete problemData;
    }

    // barrier of single construct

    // This stops the timer by printing DONE.
    Utility::stopTimer();

    return 0;
}