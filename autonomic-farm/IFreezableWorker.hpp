#ifndef I_FREEZABLE_WORKER_HPP
#define I_FREEZABLE_WORKER_HPP

#include <thread>

using namespace std;

// A worker that run on its own thread
class IFreezableWorker
{

protected:
    // The thread where this worker runs on
    thread thread_;
    // The code to be executed in the thread
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