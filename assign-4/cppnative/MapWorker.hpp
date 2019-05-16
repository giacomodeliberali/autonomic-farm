#pragma once
#include <iostream>
#include <vector>
#include "ReduceWorker.hpp"
#include "../Timer.hpp"
#include <thread>

using namespace std;

template <class TOut, class TIn, class TKey>
class MapWorker
{
private:
    function<pair<TOut, TKey> *(TIn)> mapFun;
    function<int(TKey)> hashFun;
    function<TOut(TOut, TOut)> reduceFun;
    thread *tid;
    unordered_map<TKey, TOut> container;

    vector<TIn> input;
    int start;
    int end;
    vector<ReduceWorkder<TOut, TIn, TKey> *> reducers;

public:
    MapWorker(
        function<pair<TOut, TKey> *(TIn)> mapFun,
        function<int(TKey)> hashFun,
        function<TOut(TOut, TOut)> reduceFun) : mapFun(mapFun),
                                                hashFun(hashFun),
                                                reduceFun(reduceFun)
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
            //Timer t("MapWorker");
            for (int i = start; i < end; i++)
            {
                pair<TOut, TKey> *pair = mapFun(input[i]);

                // shuffle in reducers based on hash(key)
                // but first compute a local reduce, to decrease comunication overhead

                auto key = pair->second;
                if (container.find(key) != container.end())
                {
                    // exists
                    container[key] = this->reduceFun(container[key], pair->first);
                }
                else
                {
                    // do not exist
                    container.insert(make_pair(key, pair->first));
                }
            }
            for (auto &p : container)
            {
                reducers[hashFun(p.first) % reducers.size()]->add(new std::pair(p.first, p.second));
            }
            for (auto &red : reducers)
                red->add(NULL);
        });
    }

    void joinThread()
    {
        tid->join();
    }
};