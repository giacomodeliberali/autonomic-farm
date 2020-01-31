#ifndef DEFAULT_WORKER_HPP
#define DEFAULT_WORKER_HPP

#include <functional>
#include <vector>
#include <iostream>
#include "IFreezableWorker.hpp"
#include "WorkerPool.hpp"
#include "Flags.hpp"

using namespace std;

template <typename TIN, typename TOUT>
class DefaultWorker : public IFreezableWorker
{

private:
    void wait_for_task()
    {
        unique_lock<mutex> lock(this->task_mutex);
        if (task_ == nullptr)
            this->task_condition.wait(lock, [&] { return this->task_ != nullptr; });
    }

protected:
    mutex task_mutex;
    condition_variable task_condition;
    // task to be computed
    TIN *task_ = nullptr;

    // The pool that spawned me
    WorkerPool<TIN, TOUT> *pool_;

    // The function to compute
    function<TOUT *(TIN *)> func_;

    void run() override
    {
        //cout << "\t\t[Worker] run()" << endl;

        bool eos = false;

        while (!eos)
        {
            wait_for_task();

            if (task_ == (TIN *)END_OF_STREAM)
            {
                eos = true;
                pool_->collect(this, (TOUT *)END_OF_STREAM);
                continue;
            }

            // Compute result
            auto result = func_(task_);

            // Set to null
            task_ = nullptr;

            // Notify to pool
            pool_->collect(this, result);
        }
    }

public:
    DefaultWorker(WorkerPool<TIN, TOUT> *pool, function<TOUT *(TIN *)> func) : pool_(pool), func_(func)
    {
    }

    void accept(TIN *task)
    {        
        unique_lock<mutex> lock(this->task_mutex);
        task_ = task;
        this->task_condition.notify_one();
    }
};

#endif