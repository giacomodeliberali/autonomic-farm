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

using namespace std;

template <typename TIN, typename TOUT>
class Monitor
{
private:
    // The pool to monitor
    WorkerPool<TIN, TOUT> *pool_;

    int task_collected = 0;
    int total_collected_task = 0;

    IStrategy *strategy;

    // The mutex used to ensure the collect is called thread-safe
    mutex notify_mutex;

    chrono::high_resolution_clock::time_point monitor_start;

public:
    Monitor(WorkerPool<TIN, TOUT> *pool, float expected_throughput) : pool_(pool)
    {
        strategy = new DefaultStrategy(expected_throughput);
    }

    void init()
    {
        monitor_start = chrono::high_resolution_clock::now();
        cout << "time,throughput,nw" << endl;
    }

    // Called every time a task has been collected
    void notify()
    {
        
            unique_lock<mutex> lock(this->notify_mutex);
            task_collected++;
            total_collected_task++;
        

        auto now = chrono::high_resolution_clock::now();
        auto elapsed = now - monitor_start;
        float elapsed_milliseconds = (long int)chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

        if (elapsed_milliseconds >= MONITOR_NOTIFICATION_INTERVAL)
        {
            float actual_throughput;
            auto time = chrono::duration_cast<chrono::milliseconds>(now - monitor_start).count();
            actual_throughput = task_collected / elapsed_milliseconds;
            printf("%d,%.2f,%d\n", total_collected_task, actual_throughput, pool_->get_actual_workers_number());
            task_collected = 0;
            monitor_start = chrono::high_resolution_clock::now();

            auto cmd = strategy->get(actual_throughput);

            if(cmd == Flags::ADD_WORKER || Flags::REMOVE_WORKER)
                pool_->notify_command(cmd);
        }
    }
};

#endif