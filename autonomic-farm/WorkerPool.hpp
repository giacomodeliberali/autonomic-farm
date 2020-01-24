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

    // The total amount of spawned workers
    int total_spawned_workers;

    // The function this pool workers have to compute
    function<TOUT *(TIN *)> func_;

    // Available workers pool
    ThreadSafeQueue<DefaultWorker<TIN, TOUT> *> available_workers_pool_;

    // Freezed workers pool
    ThreadSafeQueue<DefaultWorker<TIN, TOUT> *> waiting_workers_pool_;

    // The master worker that own this pool
    MasterWorker<TIN, TOUT> *master_;

    // The mutex used to lock the pool until all threads have joined
    mutex join_all_mutex;

    // The condition variable used to wait until all threads have completed (also by EOS)
    condition_variable all_joined_condition;

public:
    WorkerPool(MasterWorker<TIN, TOUT> *master, int nw, function<TOUT *(TIN *)> func) : master_(master), func_(func), initial_nw_(nw)
    {
        for (auto i = 0; i < initial_nw_; i++)
            this->add_worker();
    }

    // Assign a task to free worker or waits until one is available.
    void assign(TIN *task)
    {
        // pop a free worker or wait
        auto worker = available_workers_pool_.pop();
        available_workers_pool_.notify();

        // give it a task
        worker->accept(task);
    }

    // Collect the result from a worker and make it availabe again to receive another task.
    void collect(DefaultWorker<TIN, TOUT> *worker, TOUT *result)
    {
        if (result != (TOUT *)END_OF_STREAM)
        {
            available_workers_pool_.push(worker);
            available_workers_pool_.notify();
            master_->collect(result);
            all_joined_condition.notify_one();
        }
    }

    // Joins all workers and return their number
    int join_all()
    {
        unique_lock<mutex> lock(this->join_all_mutex);

        int total_oes_sent = 0;

        int available_pool_size = available_workers_pool_.size();
        int waiting_pool_size = waiting_workers_pool_.size();

        if (available_pool_size != (total_spawned_workers - waiting_pool_size))
            this->all_joined_condition.wait(lock, [&] {
                available_pool_size = available_workers_pool_.size();
                return available_pool_size == (total_spawned_workers - waiting_pool_size);
            });

        if (waiting_pool_size > 0)
        {
            auto workers = waiting_workers_pool_.pop_all();
            for (auto worker : workers)
            {
                worker->accept((TIN *)END_OF_STREAM);
                worker->join();
                total_oes_sent++;
            }
        }

        auto workers = available_workers_pool_.pop_all();
        for (auto worker : workers)
        {
            if (worker)
                worker->accept((TIN *)END_OF_STREAM);
            worker->join();
            total_oes_sent++;
        }

        return total_oes_sent;
    }

    int get_actual_workers_number()
    {
        return available_workers_pool_.size();
    }

    void add_worker()
    {
        if (total_spawned_workers >= 8)
            return;

        if (!waiting_workers_pool_.is_empty())
        {
            auto worker = waiting_workers_pool_.pop();
            available_workers_pool_.push(worker);
        }
        else
        {
            auto worker = new DefaultWorker<TIN, TOUT>(this, func_);
            available_workers_pool_.push(worker);
            worker->start();
            total_spawned_workers++;
        }
        available_workers_pool_.notify();
        waiting_workers_pool_.notify();
    }

    void remove_worker()
    {
        int pool_size = available_workers_pool_.size();
        if (pool_size <= 1)
            return;

        if (!available_workers_pool_.is_empty())
        {
            auto worker = available_workers_pool_.pop();
            waiting_workers_pool_.push(worker);
        }
        available_workers_pool_.notify();
        waiting_workers_pool_.notify();
    }
};

#endif