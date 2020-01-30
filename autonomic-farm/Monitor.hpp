#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "IFreezableWorker.hpp"
#include "DefaultWorker.hpp"
#include "WorkerPool.hpp"
#include "Flags.hpp"
#include "Constants.hpp"
#include "IStrategy.hpp"
#include "DefaultStrategy.hpp"
#include <iostream>

#define MONITOR_THROUGHPUT_WINDOW 10.0

using namespace std;

template <typename TIN, typename TOUT>
class Monitor
{
private:
    // The pool to monitor
    WorkerPool<TIN, TOUT> *pool_;

    int task_collected = 0;
    int total_collected_task = 0;
    float prev_throughput_ = 0;
    float expected_throughput_;

    IStrategy *strategy;

    // The mutex used to ensure the collect is called thread-safe
    mutex notify_mutex;

    chrono::high_resolution_clock::time_point monitor_start;

public:
    Monitor(WorkerPool<TIN, TOUT> *pool, float expected_throughput) : pool_(pool), expected_throughput_(expected_throughput)
    {
        strategy = new DefaultStrategy(expected_throughput);
    }

    void init()
    {
        monitor_start = chrono::high_resolution_clock::now();
        // window, expected_t, actural_t, workers_n, avg, avg_t, trend, trend_t
        // printf("%d,%.2f,%.2f,%d,%.2f,%d\n", total_collected_task, expected_throughput_, actual_throughput, actual_workers_number, elapsed, task_collected);
        cout << "tasks,expected_throughput,actual_throughput,nw" << endl;
    }

    // Called every time a task has been collected
    void notify()
    {
        unique_lock<mutex> lock(this->notify_mutex);
        task_collected++;
        total_collected_task++;

        auto now = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration_cast<std::chrono::microseconds>(now - monitor_start).count() / 1000.0;
        float actual_throughput;
        int old_task = task_collected;

        if (elapsed >= MONITOR_THROUGHPUT_WINDOW) 
        {
            // compute throughput
            actual_throughput = task_collected / elapsed;
            prev_throughput_ = actual_throughput;
            task_collected = 0;
            monitor_start = chrono::high_resolution_clock::now();
        }
        else
        {
            actual_throughput = prev_throughput_;
        }

        auto actual_workers_number = pool_->get_actual_workers_number();
        auto cmd = strategy->get(actual_throughput, actual_workers_number);

        printf("%d,%.2f,%.2f,%d\n", total_collected_task, expected_throughput_, actual_throughput, actual_workers_number);
        // printf("%d,%.2f,%.2f,%d\n", total_collected_task, expected_throughput_, actual_throughput, actual_workers_number);

        if (FlagUtils::is(cmd, ADD_WORKER) || FlagUtils::is(cmd, REMOVE_WORKER))
            pool_->notify_command(cmd);

        if (FlagUtils::is(cmd, WINDOW_FULL))
        {
            //
        }
    }
};

#endif