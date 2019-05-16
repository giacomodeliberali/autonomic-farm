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
    unordered_map<TKey, TOut> container;

    int mapWorkersCount;
    ThreadSafeQueue<pair<TKey, TOut> *> queue;
    thread *tid;
    int eosReceived = 0;

public:
    ReduceWorkder(function<TOut(TOut, TOut)> reduceFun,
                  int mapWorkersCount) : reduceFun(reduceFun),
                                         mapWorkersCount(mapWorkersCount)
    {
        //queue = ThreadSafeQueue<pair<TOut, TKey> *>();
    }

    void add(pair<TKey, TOut> *p)
    {
        // group by key then reduce
        queue.push(p);
    }

    void start()
    {
        tid = new thread([&]() {
            //Timer t("ReduceWorker");
            bool eos = false;

            while (!eos)
            {
                pair<TKey, TOut> *p = queue.pop();

                if (p == NULL)
                {
                    eosReceived++;
                    if (eosReceived == mapWorkersCount)
                        eos = true;
                    continue;
                }

                TKey key = p->first;
                TOut value = p->second;

                auto found = container.find(key);
                if (found != container.end())
                {
                    // exists
                    auto reduced = this->reduceFun(container[key], value);
                    container[key] = reduced;
                }
                else
                {
                    // do not exist
                    container.insert(make_pair(key, value));
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

        for (auto &p : container)
        {
            partialResults.push_back(new pair(p.second, p.first));
        }

        return partialResults;
    }
};