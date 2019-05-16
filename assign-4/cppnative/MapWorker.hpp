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
    unordered_map<int, pair<TOut, TKey> *> container;

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
                this_thread::sleep_for(std::chrono::milliseconds(1));
                auto pair = mapFun(input[i]);

                // shuffle in reducers based on hash(key)
                // but first compute a local reduce, to reduce comunication overhead
                auto hash = hashFun(pair->second);
                auto key = container.find(hash);
                if (key != container.end())
                {
                    // exists
                    auto reduced = this->reduceFun(container[hash]->first, pair->first);
                    container[hash] = new std::pair(reduced, pair->second);
                }
                else
                {
                    // do not exist
                    container.insert(std::pair(hash, pair));
                }
            }
            for (auto &p : container)
            {
                auto hash = p.first;
                reducers[hash % reducers.size()]->add(p.second, p.first);
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