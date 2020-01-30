#ifndef WORKER_queue_HPP
#define WORKER_queue_HPP

#include "DefaultWorker.hpp"
#include "MasterWorker.hpp"
#include "ThreadSafeQueue.hpp"
#include "Flags.hpp"
#include <vector>
#include <iostream>
#include <thread>

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
    int total_spawned_workers = 0;

    int max_concurrency;

    ThreadSafeQueue<int> monitor_commands;

    thread commands_thread;

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
        max_concurrency = thread::hardware_concurrency();
        for (auto i = 0; i < initial_nw_; i++)
        {
            auto worker = new DefaultWorker<TIN, TOUT>(this, func_);
            available_workers_pool_.push(worker);
            worker->start();
            total_spawned_workers++;
        }
        commands_thread = thread(&WorkerPool::manage_commands, this);
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

        notify_command(END_OF_STREAM);

        if (commands_thread.joinable())
            commands_thread.join();

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
            worker->accept((TIN *)END_OF_STREAM);
            worker->join();
            total_oes_sent++;
        }

        return total_oes_sent;
    }

    int get_actual_workers_number()
    {
        int waiting_pool_size = waiting_workers_pool_.size();

        return total_spawned_workers - waiting_pool_size;
    }

    void notify_command(int cmd)
    {
        monitor_commands.push(cmd);
        monitor_commands.notify();
    }

    void remove_worker()
    {

        if (get_actual_workers_number() < 2)
            return;

        auto worker = available_workers_pool_.pop();
        waiting_workers_pool_.push(worker);
        waiting_workers_pool_.notify();
    }

    void add_worker()
    {
        if (!waiting_workers_pool_.is_empty())
        {
            auto worker = waiting_workers_pool_.pop();
            available_workers_pool_.push(worker);
        }
        else
        {
            if (total_spawned_workers >= max_concurrency)
                return;

            auto worker = new DefaultWorker<TIN, TOUT>(this, func_);
            available_workers_pool_.push(worker);
            worker->start();
            total_spawned_workers++;
        }
        available_workers_pool_.notify();
    }

    void manage_commands()
    {
        bool eos = false;
        while (!eos)
        {
            int cmd = monitor_commands.pop();
            if (cmd == END_OF_STREAM)
            {
                eos = true;
                continue;
            }

            if (FlagUtils::is(cmd, ADD_WORKER))
            {
                add_worker();
            }
            else if (FlagUtils::is(cmd, REMOVE_WORKER))
            {
                remove_worker();
            }
        }
    }
};

#endif