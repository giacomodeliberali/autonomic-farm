#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include "ThreadSafeQueue.hpp"
#include "../Timer.hpp"
#include <thread>

using namespace std;

template <class TOut, class TIn, class TKey>
class ReduceWorkder
{
private:
    function<TOut(TOut, TOut)> reduceFun;
    unordered_map<int, pair<TOut, TKey> *> container;

    int mapWorkersCount;
    ThreadSafeQueue<pair<pair<TOut, TKey> *, int>> *queue;
    thread *tid;
    int eosReceived = 0;

public:
    ReduceWorkder(function<TOut(TOut, TOut)> reduceFun,
                  int mapWorkersCount) : reduceFun(reduceFun),
                                         mapWorkersCount(mapWorkersCount)
    {
        queue = new ThreadSafeQueue<pair<pair<TOut, TKey> *, int>>();
    }

    void add(pair<TOut, TKey> *p, int hash)
    {
        // group by key then reduce
        queue->push(pair(p, hash));
    }

    void start()
    {
        tid = new thread([&]() {
            //Timer t("ReduceWorker");
            bool eos = false;

            while (!eos)
            {
                auto value = queue->pop();

                auto hash = value.second;
                auto p = value.first;

                if (p == NULL && hash == 0)
                {
                    eosReceived++;
                    if (eosReceived == mapWorkersCount)
                        eos = true;
                    continue;
                }

                auto key = container.find(hash);
                if (key != container.end())
                {
                    // exists
                    auto reduced = this->reduceFun(container[hash]->first, p->first);
                    container[hash] = new pair(reduced, p->second);
                }
                else
                {
                    // do not exist
                    container.insert(pair(hash, p));
                }
            }
        });
    }

    void join()
    {
        tid->join();
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