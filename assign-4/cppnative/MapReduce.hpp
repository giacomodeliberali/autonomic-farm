#pragma once
#include <iostream>
#include <vector>
#include "ReduceWorker.hpp"
#include "MapWorker.hpp"

using namespace std;

template <class TOut, class TIn, class TKey>
class MapReduce
{
private:
    function<pair<TOut, TKey> *(TIn)> mapFun;
    function<TOut(TOut, TOut)> reduceFun;
    function<int(TKey)> hashFun;

    int mapWorkersCount;
    int reduceWorkersCount;
    vector<MapWorker<TOut, TIn, TKey> *> mapWorkers;
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
        {
            auto reduceWorker = new ReduceWorkder<TOut, TIn, TKey>(reduceFun, mapWorkersCount);
            reduceWorkers.push_back(reduceWorker);
        }

        int chunkStart = 0;
        int mapChunksCount = input.size() / mapWorkersCount;
        int chunkEnd = chunkStart + mapChunksCount;

        for (int i = 0; i < mapWorkersCount; i++)
        {
            auto mapWorker = new MapWorker<TOut, TIn, TKey>(mapFun, hashFun);
            mapWorker->initialize(input, chunkStart, chunkEnd, reduceWorkers);
            mapWorkers.push_back(mapWorker);
            chunkStart = chunkEnd;
            chunkEnd = chunkStart + mapChunksCount;
            if (chunkEnd + mapChunksCount > input.size())
                chunkEnd = input.size();
        }

        for (auto &reduceWorker : reduceWorkers)
            reduceWorker->start();

        for (auto &mapWorker : mapWorkers)
            mapWorker->startThread();

        for (auto &mapWorker : mapWorkers)
            mapWorker->joinThread();

        for (auto &reduceWorker : reduceWorkers)
            reduceWorker->join();

        // aggregate partial results
        auto result = vector<pair<TOut, TKey> *>();
        for (int i = 0; i < reduceWorkersCount; i++)
        {
            auto partials = reduceWorkers[i]->getPartialResults();
            result.insert(result.end(), partials.begin(), partials.end());
        }

        return result;
    }
};