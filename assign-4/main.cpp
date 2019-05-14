
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

using namespace std;

template <class TOut, class TIn, class TKey>
class ReduceWorkder
{
private:
    function<TOut(TOut, TOut)> reduceFun;
    unordered_map<int, pair<TOut, TKey> *> container;

public:
    ReduceWorkder(function<TOut(TOut, TOut)> reduceFun) : reduceFun(reduceFun)
    {
    }

    void add(pair<TOut, TKey> *p, int hash)
    {
        //TODO: put in a thread safe queue
        // group by key then reduce
        auto key = container.find(hash);
        if (key != container.end())
        {
            // exists
            auto reduced = reduceFun(container[hash]->first, p->first);
            container[hash] = new pair(reduced, p->second);
        }
        else
        {
            // do not exist
            container.insert(pair(hash, p));
        }
    }

    vector<pair<TOut, TKey> *> getPartialResults()
    {
        vector<pair<TOut, TKey> *> partialResults;

        for (auto reducedValue : container)
        {
            partialResults.push_back(reducedValue.second);
        }

        return partialResults;
    }
};
 
template <class TOut, class TIn, class TKey>
class MapWorker
{
private:
    function<pair<TOut, TKey> *(TIn)> mapFun;
    function<int(TKey)> hashFun;

public:
    MapWorker(
        function<pair<TOut, TKey> *(TIn)> mapFun,
        function<int(TKey)> hashFun) : mapFun(mapFun),
                                       hashFun(hashFun)
    {
    }

    void initialize(vector<TIn> input, int start, int end, vector<ReduceWorkder<TOut, TIn, TKey> *> reducers)
    {
        for (int i = start; i < end; i++)
        {
            auto pair = mapFun(input[i]);

            // shuffle in reducers based on hash(key)
            auto hash = hashFun(pair->second);
            reducers[hash % reducers.size()]->add(pair, hash);
        }   
    }
};

template <class TOut, class TIn, class TKey>
class MapReduce
{
private:
    function<pair<TOut, TKey> *(TIn)> mapFun;
    function<TOut(TOut, TOut)> reduceFun;
    function<int(TKey)> hashFun;

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

    void set_hash(function<int(TKey)> hashFun)
    {
        this->hashFun = hashFun;
    }

    vector<pair<TOut, TKey> *> run(vector<TIn> input)
    {
        for (int i = 0; i < reduceWorkersCount; i++)
            reduceWorkers.push_back(new ReduceWorkder<TOut, TIn, TKey>(reduceFun));

        int chunkStart = 0;
        int mapChunksCount = input.size() / mapWorkersCount;
        int chunkEnd = chunkStart + mapChunksCount;

        for (int i = 0; i < mapWorkersCount; i++)
        {
            auto mapWorker = MapWorker<TOut, TIn, TKey>(mapFun, hashFun);
            mapWorker.initialize(input, chunkStart, chunkEnd, reduceWorkers);
            mapWorkers.push_back(mapWorker);
            chunkStart = chunkEnd;
            chunkEnd = chunkStart + mapChunksCount;
            if (chunkEnd + mapChunksCount > input.size())
                chunkEnd = input.size();
        }

        auto result = vector<pair<TOut, TKey> *>();

        for (int i = 0; i < reduceWorkersCount; i++)
        {
            auto partials = reduceWorkers[i]->getPartialResults();
            result.insert(result.end(), partials.begin(), partials.end());
        }

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

    mapReduce.set_hash([](auto key) {
        return key % 4;
    });

    mapReduce.set_nw(10, 4);

    vector<int> vec;
    for (int i = 0; i < 100; i++)
        vec.push_back(i);

    for (auto val : mapReduce.run(vec))
    {
        cout << val->first << " ";
    }
    cout << endl;
}