#pragma once
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

    vector<pair<TOut, TKey> *> getResults()
    {
        vector<pair<TOut, TKey> *> partialResults;

        for (auto reducedValue : container)
        {
            partialResults.push_back(reducedValue.second);
        }

        return partialResults;
    }
};