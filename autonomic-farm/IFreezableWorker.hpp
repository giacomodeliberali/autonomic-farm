#ifndef I_FREEZABLE_WORKER_HPP
#define I_FREEZABLE_WORKER_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>

using namespace std;

// A worker that run on its own thread that can be freezed until a condition is met
class IFreezableWorker
{

protected:
    // The thread where this worker runs on
    thread thread_;
    // The code to be executed when not in freeze state
    virtual void run() = 0;

public:
    // Spawn a new thread with this worker code
    void start()
    {
        thread_ = thread(&IFreezableWorker::run, this);
    }

    // Join the worker's thread
    virtual void join()
    {
        thread_.join();
    }
};

#endif