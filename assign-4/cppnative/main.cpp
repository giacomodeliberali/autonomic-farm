
#include <iostream>
#include <vector>
#include "MapReduce.hpp"
#include "../Timer.hpp"

using namespace std;

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        cout << "Usage is: [nw] [vec_len] [seed] " << endl;
        return 0;
    }

    int nw = atoi(argv[1]);
    int vecLength = atoi(argv[2]);
    int seed = atoi(argv[3]);

    auto mapReduce = MapReduce<long, long, long>();
    mapReduce.set_map([nw](auto input) {
        return new pair(
            input * 2, // double the number
            input % 8  // even and odd numbers
        );
    });
    mapReduce.set_reduce([](auto left, auto right) {
        return left + right; // sum the event and odd numbers
    });

    mapReduce.set_hash([](auto key) {
        return key;
    });

    mapReduce.set_nw(nw, nw);

    vector<long> vec;
    srand (seed);
    for (long i = 1; i <= vecLength; i++)
        vec.push_back(rand() % 500);

    vector<pair<long, long> *> results;
    {
        auto t = Timer("Parallel map reduce");
        results = mapReduce.run(vec);
    }
    for (auto val : results)
    {
        cout << "[Key: " << val->second << "] = " << val->first << endl;
    }
}