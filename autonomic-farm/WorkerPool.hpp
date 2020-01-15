#ifndef WORKER_queue_HPP
#define WORKER_queue_HPP

#include "DefaultWorker.hpp"
#include "MasterWorker.hpp"
#include "ThreadSafeQueue.hpp"
#include <vector>
#include <iostream>
#define END_OF_STREAM 0xffff

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
        cout << "\t[Pool] Created" << endl;
        for (auto i = 0; i < nw; i++)
        {
            auto worker = new DefaultWorker<TIN, TOUT>(this, func);
            pool_.push(worker);
            worker->start();
        }
    }

    void assign(TIN *task)
    {
        // pop a free worker
        cout << "\t[Pool] waiting for an available worker..." << endl;
        auto worker = pool_.pop();
        cout << "\t   [Pool] ...found!" << endl;

        // give it a task
        worker->accept(task);
    }

    void collect(DefaultWorker<TIN, TOUT> *worker, TOUT *result)
    {
        pool_.push(worker);
        cout << "\t[Pool] Pushing an available worker. Now availables = " << pool_.size() << endl;
        master_->collect(result);
    }

    // Joins all workers and return their number
    int join_all()
    {
        int count = 0;

        for (auto worker : pool_.pop_all())
        {
            worker->accept((TIN *)END_OF_STREAM);
            worker->join();
            count++;
        }

        return count;
    }
};

#endif