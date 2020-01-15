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
private:
    bool must_stop = false;

    // Set the freezed flag acquiring the lock
    void setFreezed(bool val)
    {
        unique_lock<mutex> lock(this->freeze_mutex_);
        isFreezed_ = val;
    }

    // Manages the lock flag and lifecycles events
    void _run()
    {
        bool is_freezed = false;
        {
            unique_lock<mutex> lock(this->freeze_mutex_);
            is_freezed = isFreezed_;
        }

        if (is_freezed)
        {
            unique_lock<mutex> lock(this->freeze_mutex_);
            this->onBeforeFreezing();
            this->freeze_condition_.wait(lock, [=] { return this->isFreezed_ == false; });
            this->onAfterFreezing();
        }
        else
        {
            this->run();
        }
    }

protected:
    // The lock used to freeze/unfreeze
    mutex freeze_mutex_;
    // The conditional variable for the freeze/unfreeze
    condition_variable freeze_condition_;
    // Indicate if this worker is freezed
    bool isFreezed_;
    // The thread where this worker runs on
    thread thread_;

    // ### Virtual methods ###

    // The code to be executed before being freezed
    virtual void onBeforeFreezing(){};
    // The code to be executed after having being freezed
    virtual void onAfterFreezing(){};
    // The code to be executed when not in freeze state
    virtual void run() = 0;

public:
    // Indicate if the current worker is freezed or not
    bool isFreezed()
    {
        unique_lock<mutex> lock(this->freeze_mutex_);
        return isFreezed_;
    }

    // Freeze the current worker
    void freeze()
    {
        setFreezed(true);
    }

    // Unfreeze the current worker
    void unfreeze()
    {
        setFreezed(false);
        this->freeze_condition_.notify_one();
    }

    // Spawn a new thread with this worker code
    void start()
    {
        thread_ = thread(&IFreezableWorker::_run, this);
    }

    // Join the worker's thread
    void join()
    {
        thread_.join();
    }
};

#endif