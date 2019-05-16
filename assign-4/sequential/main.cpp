
#include <iostream>
#include <vector>
#include "MapReduce.hpp"
#include "../Timer.hpp"
#include "../shared.hpp"

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

    srand(seed);

    auto mapReduce = MapReduce<long, long, long>();
    mapReduce.set_map(mapFun);
    mapReduce.set_reduce(reduceFun);
    mapReduce.set_hash(hashFun);

    vector<long> vec;

    for (long i = 1; i <= vecLength; i++)
        vec.push_back(rand() % 500);

    {
        Timer t("Seq map reduce (no local reduce) ");
        mapReduce.run(vec);
    }
}