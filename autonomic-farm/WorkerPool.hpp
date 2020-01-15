#ifndef WORKER_queue_HPP
#define WORKER_queue_HPP

#include "DefaultWorker.hpp"
#include "MasterWorker.hpp"
#include "ThreadSafeQueue.hpp"
#include <vector>
#include <iostream>

using namespace std;

#pragma region Template declarations

// MasterWorker
template <typename TIN, typename TOUT>
class MasterWorker;

// DefaultWorker
template <typename TIN, typename TOUT>
class DefaultWorker;

#pragma endregion Template declarations

template <typename TIN, typename TOUT>
class WorkerPool
{

private:
    // Workers pool
    ThreadSafeQueue<DefaultWorker<TIN, TOUT> *> pool_;

    // The master worker that own this pool
    MasterWorker<TIN, TOUT> *master_;

public:
    WorkerPool(MasterWorker<TIN, TOUT> *master, int nw, function<TOUT *(TIN *)> func) : master_(master)
    {
        cout << "[Pool] Created" << endl;
        for (auto i = 0; i < nw; i++)
        {
            auto worker = new DefaultWorker<TIN, TOUT>(this, func);
            pool_.push(worker);
            worker->start();
        }
    }

    bool has_available_worker()
    {
        return pool_.is_empty();
    }

    bool assign(TIN *task)
    {
        if (!has_available_worker())
        {
            return false;
        }

        // pop a free worker
        auto worker = pool_.pop();

        // give it a task
        worker->accept(task);

        return true;
    }

    void collect(DefaultWorker<TIN, TOUT> *worker, TOUT *result)
    {
        pool_.push(worker);
        master_->collect(result);
    }

    // Joins all workers and return their number
    int join_all()
    {
        int count = 0;

        for (auto worker : pool_.pop_all())
        {
            worker->join();
            count++;
        }

        return count;
    }
};

#endif