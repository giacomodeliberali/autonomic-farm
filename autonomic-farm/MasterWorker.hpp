#ifndef MASTER_WORKER_HPP
#define MASTER_WORKER_HPP

#include "IFreezableWorker.hpp"
#include "DefaultWorker.hpp"
#include "IEmitter.hpp"
#include "WorkerPool.hpp"
#include "Collector.hpp"
#include "Monitor.hpp"
#include <vector>
#include <iostream>

using namespace std;

template <typename TIN, typename TOUT>
class MasterWorker
{
private:
    IEmitter<TIN> *emitter_;
    WorkerPool<TIN, TOUT> *pool_;
    Collector<TOUT> *collector_;
    Monitor<TIN, TOUT> *monitor_;

public:
    MasterWorker(IEmitter<TIN> *emitter, int nw, function<TOUT *(TIN *)> func, float expected_throughput) : emitter_(emitter)
    {
        pool_ = new WorkerPool(this, nw, func);
        monitor_ = new Monitor(pool_, expected_throughput);
        collector_ = new Collector<TOUT>();
    }

    void collect(TOUT *result)
    {
        monitor_->notify();
        collector_->collect(result);
    }

    // The code to be executed from the master worker
    MasterWorker<TIN, TOUT> *run()
    {
        monitor_->init();
        bool has_more_items = true;
        while (has_more_items)
        {
            auto next = emitter_->get_next();
            if (next != nullptr)
                this->pool_->assign(next);
            else
                has_more_items = false;
        }

        auto joined_workers = pool_->join_all();
        // cout << "[Master] joined all (" << joined_workers << ")" << endl;

        return this;
    };

    vector<TOUT *> *get_results()
    {
        return this->collector_->get_results();
    }
};

#endif