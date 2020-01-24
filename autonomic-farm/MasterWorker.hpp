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

    // Stick current thread on a core
    void stick()
    {
        /*         cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(sched_getcpu(), &cpuset);

        pthread_t current_thread = pthread_self();
        pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset); */
    }

public:
    MasterWorker(IEmitter<TIN> *emitter, int nw, function<TOUT *(TIN *)> func) : emitter_(emitter)
    {
        pool_ = new WorkerPool(this, nw, func);
        monitor_ = new Monitor(pool_);
        collector_ = new Collector<TOUT>();
        this->stick();
    }

    void collect(TOUT *result)
    {
        collector_->collect(result);
        monitor_->notify();
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
        cout << "[Master] joined all (" << joined_workers << ")" << endl;

        return this;
    };

    vector<TOUT *> *get_results()
    {
        return this->collector_->get_results();
    }
};

#endif