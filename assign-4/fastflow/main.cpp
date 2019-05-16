
#include <iostream>
#include <vector>
#include "Workers.hpp"
#include "../Timer.hpp"

using namespace std;

int main(int argc, char *argv[])
{

    if (argc != 4)
    {
        cout << "Usage is: [nw] [vec_len] [seed] " << endl;
        return 0;
    }

    int nw = atoi(argv[1]);
    int vecLength = atoi(argv[2]);
    int seed = atoi(argv[3]);

    vector<ff_node *> mapWorkers;
    vector<ff_node *> reduceWorkers;

    auto mapFun = ([](auto input) {
        //this_thread::sleep_for(std::chrono::milliseconds(100));
        return new pair(
            input * 2, // double the number
            input % 8  // even and odd numbers
        );
    });
    auto reduceFun = ([](auto left, auto right) {
        return left + right; // sum the event and odd numbers
    });

    auto hashFun = ([](auto key) {
        return key;
    });

    vector<int> vec;
    srand(seed);
    for (int i = 1; i <= vecLength; i++)
        vec.push_back(rand() % 500);

    int chunkStart = 0;
    int mapChunksCount = vecLength / nw;
    int chunkEnd = chunkStart + mapChunksCount;
    for (int i = 0; i < nw; i++)
    {
        auto mapWorker = new ffMapWorker<int, int, int>(vec, chunkStart, chunkEnd, mapFun, hashFun);
        mapWorkers.push_back(mapWorker);
        chunkStart = chunkEnd;
        chunkEnd = chunkStart + mapChunksCount;
        if (chunkEnd + mapChunksCount > vecLength)
            chunkEnd = vecLength;

        auto reduceWorker = new ffReduceWorker<int, int, int>(reduceFun, hashFun);
        reduceWorkers.push_back(reduceWorker);
    }

    ff_a2a a2a;
    a2a.add_firstset(mapWorkers);
    a2a.add_secondset(reduceWorkers);

    ffTime(START_TIME);

    if (a2a.run_and_wait_end() < 0)
    {
        error("running a2a\n");
        return -1;
    }

    ffTime(STOP_TIME);
    std::cout << "Time: " << ffTime(GET_TIME) << " (ms)\n";
    std::cout << "A2A Time: " << a2a.ffTime() << " (ms)\n";
}