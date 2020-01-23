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
    // The initial value of concurrency
    int initial_nw_;

    // Workers pool
    ThreadSafeQueue<DefaultWorker<TIN, TOUT> *> pool_queue_;

    // The master worker that own this pool
    MasterWorker<TIN, TOUT> *master_;

    // The mutex used to ensure the collect is called thread-safe
    mutex collect_mutex;

    // The mutex used to lock the pool until all threads have joined
    mutex join_all_mutex;

    // The condition variable used to wait until all threads have completed (also by EOS)
    condition_variable all_joined_condition;

public:
    WorkerPool(MasterWorker<TIN, TOUT> *master, int nw, function<TOUT *(TIN *)> func) : master_(master), initial_nw_(nw)
    {
        for (auto i = 0; i < initial_nw_; i++)
        {
            auto worker = new DefaultWorker<TIN, TOUT>(this, func, i + 1);
            pool_queue_.push(worker);
            worker->start();
        }
        pool_queue_.notify();
    }

    // Assign a task to free worker or waits until one is available.
    void assign(TIN *task)
    {
        // pop a free worker or wait
        auto worker = pool_queue_.pop();
        pool_queue_.notify();

        // give it a task
        worker->accept(task);
    }

    // Collect the result from a worker and make it availabe again to receive another task.
    void collect(DefaultWorker<TIN, TOUT> *worker, TOUT *result)
    {
        pool_queue_.push(worker);
        if (result != (TOUT *)END_OF_STREAM)
        {
            {
                // ensure the collect is called in a thread-safe fashon
                unique_lock<mutex> lock(this->collect_mutex);
                master_->collect(result);
            }
        }

        pool_queue_.notify();
        all_joined_condition.notify_one();
    }

    // Joins all workers and return their number
    int join_all()
    {
        unique_lock<mutex> lock(this->join_all_mutex);
        if (pool_queue_.size() != initial_nw_)
            this->all_joined_condition.wait(lock, [=] {
                int pool_size = pool_queue_.size();
                return pool_size == initial_nw_;
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