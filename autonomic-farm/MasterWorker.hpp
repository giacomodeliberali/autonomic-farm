#ifndef MASTER_WORKER_HPP
#define MASTER_WORKER_HPP

#include "IFreezableWorker.hpp"
#include "DefaultWorker.hpp"
#include "IEmitter.hpp"
#include "WorkerPool.hpp"
#include <vector>
#include <iostream>

using namespace std;

template <typename TIN, typename TOUT>
class MasterWorker
{
private:
    IEmitter<TIN> *emitter_;
    WorkerPool<TIN, TOUT> *pool_;

    // The mutex used to ensure the collect is called thread-safe
    mutex collect_mutex;

    // Stick current thread on a core
    void stick()
    {
/*         cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(sched_getcpu(), &cpuset);

        pthread_t current_thread = pthread_self();
        pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset); */
    }

protected:
    // The code to be executed from the master worker
    void run()
    {
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
        cout << "[Master] joined all (" << joined_workers << "). Sum = " << sum << endl;
    };

public:
    MasterWorker(IEmitter<TIN> *emitter, int nw, function<TOUT *(TIN *)> func) : emitter_(emitter)
    {
        pool_ = new WorkerPool(this, nw, func);
        this->stick();
        this->run();
    }

    long long sum = 0;
    void collect(TOUT *result)
    {
        // ensure the collect is called in a thread-safe fashon
        unique_lock<mutex> lock(this->collect_mutex);
        sum += *result;
    }
};

#endif