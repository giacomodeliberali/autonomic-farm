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

public:
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

        auto reduceWorker = new ReduceWorkder<TOut, TIn, TKey>(reduceFun);

        auto mapWorker = new MapWorker<TOut, TIn, TKey>(mapFun, hashFun);
        mapWorker->initialize(input, reduceWorker);

        return reduceWorker->getResults();
    }
};