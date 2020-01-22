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

    int nw_;

    // Workers pool
    ThreadSafeQueue<DefaultWorker<TIN, TOUT> *> pool_;

    // The master worker that own this pool
    MasterWorker<TIN, TOUT> *master_;

public:
    WorkerPool(MasterWorker<TIN, TOUT> *master, int nw, function<TOUT *(TIN *)> func) : master_(master), nw_(nw)
    {
        for (auto i = 0; i < nw_; i++)
        {
            auto worker = new DefaultWorker<TIN, TOUT>(this, func, i + 1);
            pool_.push(worker);
            worker->start();
        }
        pool_.notify();
    }

    void assign(TIN *task)
    {
        // pop a free worker or wait
        auto worker = pool_.pop();
        //cout << "\t   [Pool] assign " << *task << endl;

        // give it a task
        worker->accept(task);
    }

    void collect(DefaultWorker<TIN, TOUT> *worker, TOUT *result)
    {
        pool_.push(worker);
        if (result != nullptr)
        {
            cout << "\t[Pool] collect " << *result << " from " << worker->id_ << " poolsize=" << pool_.size() << endl;
            master_->collect(result);
        }
        pool_.notify();
        task_condition.notify_one();
    }

    mutex task_mutex;
    condition_variable task_condition;
    // Joins all workers and return their number
    int join_all()
    {
        unique_lock<mutex> lock(this->task_mutex);
        if (pool_.size() != nw_)
            this->task_condition.wait(lock, [=] { return pool_.size() == nw_; });

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