
#include <iostream>
#include <vector>
#include "MapReduce.hpp"
#include "../Timer.hpp"

using namespace std;

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        cout << "Usage is: [vec_len] [seed] " << endl;
        return 0;
    }

    int vecLength = atoi(argv[1]);
    int seed = atoi(argv[2]);

    auto mapReduce = MapReduce<long, long, long>();
    mapReduce.set_map([](auto input) {
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

    vector<long> vec;
    srand (seed);
    for (long i = 1; i <= vecLength; i++)
        vec.push_back(rand() % 500);

    vector<pair<long, long> *> results;
    {
        auto t = Timer("Seq map reduce");
        results = mapReduce.run(vec);
    }
    for (auto val : results)
    {
        cout << "[Key: " << val->second << "] = " << val->first << endl;
    }
}