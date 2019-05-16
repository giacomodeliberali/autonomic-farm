
#include <iostream>
#include <vector>
#include "MapReduce.hpp"
#include "../Timer.hpp"
#include "../shared.hpp"

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

    srand(seed);

    auto mapReduce = MapReduce<long, long, long>();
    mapReduce.set_map(mapFun);
    mapReduce.set_reduce(reduceFun);
    mapReduce.set_hash(hashFun);

    mapReduce.set_nw(nw, nw);

    vector<long> vec;

    for (long i = 1; i <= vecLength; i++)
        vec.push_back(rand() % 500);

    {
        Timer t("Parallel map reduce (with local reduce)");
        mapReduce.run(vec);
    }
}