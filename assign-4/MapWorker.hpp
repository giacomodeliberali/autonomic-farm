#pragma once
#include <iostream>
#include <vector>
#include "ReduceWorker.hpp"
#include <thread>

using namespace std;

template <class TOut, class TIn, class TKey>
class MapWorker
{
private:
    function<pair<TOut, TKey> *(TIn)> mapFun;
    function<int(TKey)> hashFun;
    thread *tid;

    vector<TIn> input;
    int start;
    int end;
    vector<ReduceWorkder<TOut, TIn, TKey> *> reducers;

public:
    MapWorker(
        function<pair<TOut, TKey> *(TIn)> mapFun,
        function<int(TKey)> hashFun) : mapFun(mapFun),
                                       hashFun(hashFun)
    {
    }

    void initialize(vector<TIn> input, int start, int end, vector<ReduceWorkder<TOut, TIn, TKey> *> reducers)
    {
        this->input = input;
        this->start = start;
        this->end = end;
        this->reducers = reducers;
    }

    void startThread()
    {
        tid = new std::thread([&]() {
            for (int i = start; i < end; i++)
            {
                auto pair = mapFun(input[i]);

                // shuffle in reducers based on hash(key)
                auto hash = hashFun(pair->second);
                reducers[hash % reducers.size()]->add(pair, hash);
            }
            for (auto &red : reducers)
                red->add(NULL, 0);
        });
    }

    void joinThread()
    {
        tid->join();
    }
};