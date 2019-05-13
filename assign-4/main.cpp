
#include <iostream>
#include <vector>
#include <string>
#include <functional>

using namespace std;

template <class TOut, class TIn, class TKey>
class ReduceWorkder
{
private:
    function<TOut(TOut, TOut)> reduceFun;
    vector<pair<TOut, TKey> *> mapped;

public:
    ReduceWorkder(function<TOut(TOut, TOut)> reduceFun) : reduceFun(reduceFun)
    {
    }

    void add(pair<TOut, TKey> *pair)
    {
        mapped.push_back(pair);
    }

    void reduce()
    {
        // sort by key then reduce for each different key
        for (auto &i : mapped)
        {
            cout << reduceFun(i->first, i->first) << " ";
        }
        cout << endl;
    }
};

template <class TOut, class TIn, class TKey>
class MapWorker
{
private:
    function<pair<TOut, TKey> *(TIn)> mapFun;

public:
    MapWorker(function<pair<TOut, TKey> *(TIn)> mapFun) : mapFun(mapFun)
    {
    }

    void initialize(vector<TIn> input, int start, int end, vector<ReduceWorkder<TOut, TIn, TKey> *> reducers)
    {
        vector<pair<TOut, TKey> *> mapped;
        for (int i = start; i < end; i++)
        {
            auto pair = mapFun(input[i]);
            mapped.push_back(pair);
        }

        // shuffle in reducers based on hash(key)
        for (int i = 0; i < mapped.size(); i++)
        {
            std::hash<std::string> hasher;
            auto hash = hasher(to_string(mapped[i]->second));
            reducers[hash % reducers.size()]->add(mapped[i]);
        }
    }
};

template <class TOut, class TIn, class TKey>
class MapReduce
{
private:
    function<pair<TOut, TKey> *(TIn)> mapFun;
    function<TOut(TOut, TOut)> reduceFun;

    int mapWorkersCount;
    int reduceWorkersCount;
    vector<MapWorker<TOut, TIn, TKey>> mapWorkers;
    vector<ReduceWorkder<TOut, TIn, TKey> *> reduceWorkers;

public:
    void set_nw(int mapWorkers, int reduceWorkers)
    {
        this->mapWorkersCount = mapWorkers;
        this->reduceWorkersCount = reduceWorkers;
    }

    void set_map(function<pair<TOut, TKey> *(TIn)> mapFun)
    {
        this->mapFun = mapFun;
    }

    void set_reduce(function<TOut(TOut, TOut)> reduceFun)
    {
        this->reduceFun = reduceFun;
    }

    vector<pair<TOut, TKey>> run(vector<TIn> input)
    {
        auto result = vector<pair<TOut, TKey>>();

        for (int i = 0; i < reduceWorkersCount; i++)
            reduceWorkers.push_back(new ReduceWorkder<TOut, TIn, TKey>(reduceFun));

        int chunkStart = 0;
        int mapChunksCount = input.size() / mapWorkersCount;
        int chunkEnd = chunkStart + mapChunksCount;

        for (int i = 0; i < mapWorkersCount; i++)
        {
            auto mapWorker = MapWorker<TOut, TIn, TKey>(mapFun);
            mapWorker.initialize(input, chunkStart, chunkEnd, reduceWorkers);
            mapWorkers.push_back(mapWorker);
            chunkStart = chunkEnd;
            chunkEnd = chunkStart + mapChunksCount;
            if (chunkEnd + mapChunksCount > input.size())
                chunkEnd = input.size();
        }

        for (int i = 0; i < reduceWorkersCount; i++)
            reduceWorkers[i]->reduce();

        return result;
    }
};

int main(int argc, char *argv[])
{

    auto mapReduce = MapReduce<int, int, int>();
    mapReduce.set_map([](auto input) {
        return new pair(input * 2, input);
    });
    mapReduce.set_reduce([](auto left, auto right) {
        return left + right;
    });

    mapReduce.set_nw(10, 4);

    vector<int> vec;
    for (int i = 0; i < 100; i++)
        vec.push_back(i);

    mapReduce.run(vec);
}