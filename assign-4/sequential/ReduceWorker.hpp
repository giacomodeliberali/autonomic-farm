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
    unordered_map<TKey, TOut> container;

public:
    ReduceWorkder(function<TOut(TOut, TOut)> reduceFun) : reduceFun(reduceFun)
    {
    }

    void add(pair<TOut, TKey> *p)
    {
        auto key = container.find(p->second);
        if (key != container.end())
        {
            // exists
            auto reduced = reduceFun(container[p->second], p->first);
            container[p->second] = reduced;
        }
        else
        {
            // do not exist
            container.insert(make_pair(p->second, p->first));
        }
    }

    vector<pair<TOut, TKey> *> getResults()
    {
        vector<pair<TKey, TOut> *> partialResults;

        for (auto &p : container)
        {
            partialResults.push_back(new pair(p.second, p.first));
        }

        return partialResults;
    }
};