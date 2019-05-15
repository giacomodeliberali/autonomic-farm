#include <string>
#include <vector>
#include <iostream>
#include <ff/ff.hpp>
#include <unordered_map>

using namespace ff;
using namespace std;

template <typename TIn, typename TOut, typename TKey>
class ffMapWorker : public ff_monode_t<vector<TIn>, pair<TKey, TOut>>
{
private:
    int start;
    int end;
    vector<TIn> input;
    function<pair<TOut, TKey> *(TIn)> mapFun;
    function<int(TKey)> hashFun;

public:
    ffMapWorker(vector<TIn> input, int start, int end, function<pair<TOut, TKey> *(TIn)> mapFun, function<int(TKey)> hashFun) : input(input), start(start), end(end), mapFun(mapFun), hashFun(hashFun)
    {
    }

    pair<TKey, TOut> *svc(vector<TIn> *)
    {
        pair<TKey, TOut> *results;

        auto nw = this->get_num_outchannels();
        for (int i = start; i < end; i++)
        {
            auto pair = mapFun(input[i]);
            // shuffle in reducers based on hash(key)
            auto hash = hashFun(pair->second);
            this->ff_send_out_to(pair, hash % nw);
        }
        for (int i = 0; i < nw; i++)
            this->ff_send_out_to(NULL, i);

        return this->EOS;
    }
};

template <typename TIn, typename TOut, typename TKey>
class ffReduceWorker : public ff_minode_t<pair<TKey, TOut>>
{
private:
    function<TOut(TOut, TOut)> reduceFun;
    unordered_map<int, pair<TOut, TKey> *> container;
    function<int(TKey)> hashFun;
    int eosReceived = 0;

public:
    ffReduceWorker(function<TOut(TOut, TOut)> reduceFun, function<int(TKey)> hashFun) : reduceFun(reduceFun), hashFun(hashFun)
    {
    }

    pair<TKey, TOut> *svc(pair<TKey, TOut> *pair)
    {
        if (pair != NULL)
        {
            eosReceived++;
            auto nw = this->get_num_inchannels();
            if (eosReceived == nw)
                return this->EOS;
            return this->GO_ON;
        }
        else
        {
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
            return this->GO_ON;
        }
    }
};