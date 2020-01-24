#ifndef MONITOR_HPP
#define MONITOR_HPP

#include "IFreezableWorker.hpp"
#include "DefaultWorker.hpp"
#include "WorkerPool.hpp"
#include <iostream>

using namespace std;

template <typename TIN, typename TOUT>
class Monitor
{
private:
    // The pool to monitor
    WorkerPool<TIN, TOUT> *pool_;

    // The expected number of task per millisecond
    float expected_throughput;

    // The last calculated throughput
    float last_throughput;

    int task_collected = 0;

    // The mutex used to ensure the collect is called thread-safe
    mutex notify_mutex;

    chrono::high_resolution_clock::time_point window_start;
    chrono::high_resolution_clock::time_point monitor_start;

public:
    Monitor(WorkerPool<TIN, TOUT> *pool) : pool_(pool)
    {
        expected_throughput = 1;
    }

    void init()
    {
        monitor_start = chrono::high_resolution_clock::now();
        window_start = monitor_start;
        cout << "time,throughput,nw" << endl;
    }

    // Called every time a task has been collected
    void notify()
    {
        {
            unique_lock<mutex> lock(this->notify_mutex);
            task_collected++;
        }

        auto now = chrono::high_resolution_clock::now();
        auto elapsed = now - window_start;
        float elapsed_milliseconds = (long int)chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / 1000.0;

        if (elapsed_milliseconds >= 0.5)
        {
            float actual_throughput;
            auto time = chrono::duration_cast<chrono::milliseconds>(now - monitor_start).count();
            actual_throughput = task_collected / elapsed_milliseconds;
            last_throughput = actual_throughput;
            printf("%.2lld,%.2f,%d\n", time, actual_throughput, pool_->get_actual_workers_number());
            task_collected = 0;
            window_start = chrono::high_resolution_clock::now();

            if (actual_throughput < expected_throughput - 0.5)
            {
                // add workers
                pool_->add_worker();
            }
            else if (actual_throughput > expected_throughput + 0.5)
            {
                // freeze workers
                pool_->remove_worker();
            }
        }
    }
};

#endif