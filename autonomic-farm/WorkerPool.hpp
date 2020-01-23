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
    ThreadSafeQueue<DefaultWorker<TIN, TOUT> *> pool_queue_;

    // The master worker that own this pool
    MasterWorker<TIN, TOUT> *master_;

public:
    WorkerPool(MasterWorker<TIN, TOUT> *master, int nw, function<TOUT *(TIN *)> func) : master_(master), nw_(nw)
    {
        for (auto i = 0; i < nw_; i++)
        {
            auto worker = new DefaultWorker<TIN, TOUT>(this, func, i + 1);
            pool_queue_.push(worker);
            worker->start();
        }
        pool_queue_.notify();
    }

    void assign(TIN *task)
    {
        // pop a free worker or wait
        auto worker = pool_queue_.pop();
        pool_queue_.notify();
        //cout << "\t   [Pool] assign " << *task << endl;

        // give it a task
        worker->accept(task);
    }

    mutex collect_mutex;
    void collect(DefaultWorker<TIN, TOUT> *worker, TOUT *result)
    {
        // unique_lock<mutex> lock(this->collect_mutex);
        // some threads do not join the pool once their task is done. Why?

        pool_queue_.push(worker);
        if (result != (TOUT *)END_OF_STREAM)
        {
            // cout << "\t[Pool] collect " << *result << " from " << worker->id_ << " poolsize=" << pool_queue_.size() << endl;
            master_->collect(result);
        }

        pool_queue_.notify();
        all_joined_condition.notify_one();
    }

    mutex join_all_mutex;
    condition_variable all_joined_condition;
    // Joins all workers and return their number
    int join_all()
    {
        unique_lock<mutex> lock(this->join_all_mutex);
        if (pool_queue_.size() != nw_)
            this->all_joined_condition.wait(lock, [=] {
                int pool_size = pool_queue_.size();
                return pool_size == nw_;
            });

        int count = 0;
        auto workers = pool_queue_.pop_all();

        for (auto worker : workers)
        {
            worker->accept((TIN *)END_OF_STREAM);
            worker->join();
            count++;
        }

        return count;
    }
};

#endif