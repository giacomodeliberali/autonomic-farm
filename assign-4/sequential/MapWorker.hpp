#pragma once
#include <iostream>
#include <vector>
#include "ReduceWorker.hpp"

using namespace std;

template <class TOut, class TIn, class TKey>
class MapWorker
{
private:
    function<pair<TOut, TKey> *(TIn)> mapFun;
    function<int(TKey)> hashFun;

    vector<TIn> input;
    ReduceWorkder<TOut, TIn, TKey> *reducer;

public:
    MapWorker(
        function<pair<TOut, TKey> *(TIn)> mapFun,
        function<int(TKey)> hashFun) : mapFun(mapFun),
                                       hashFun(hashFun)
    {
    }

    void initialize(vector<TIn> input, ReduceWorkder<TOut, TIn, TKey> *reducer)
    {
        this->input = input;
        this->reducer = reducer;

        for (auto i : input)
        {
            auto pair = mapFun(input[i]);

            // shuffle in reducers based on hash(key)
            auto hash = hashFun(pair->second);
            reducer->add(pair, hash);
        }
    }
};