#ifndef DEFAULT_WORKER_HPP
#define DEFAULT_WORKER_HPP

#include "IFreezableWorker.hpp"
#include "WorkerPool.hpp"
#include <vector>
#include <iostream>

using namespace std;

template <typename TIN, typename TOUT>
class DefaultWorker : public IFreezableWorker
{
protected:
    mutex task_mutex;
    condition_variable task_condition;
    // business logic code
    function<TOUT *(TIN *)> user_task;
    // task to be computed
    TIN *task_ = NULL;

    // The pool that spawned me
    WorkerPool<TIN, TOUT> *pool_;

    // The function to compute
    function<TOUT *(TIN *)> func_;

    void run() override
    {
        cout << "[Worker] run()" << endl;
        unique_lock<mutex> lock(this->task_mutex);
        if (task_ == NULL)
            this->task_condition.wait(lock, [=] { return this->task_ != NULL; });

        // Compute result
        auto result = func_(task_);

        // Notify to pool
        pool_->collect(this, result);
    }

public:
    DefaultWorker(WorkerPool<TIN, TOUT> *pool, function<TOUT *(TIN *)> func) : pool_(pool), func_(func)
    {
        cout << "[Worker] Created" << endl;
    }

    void accept(TIN *task)
    {
        {
            unique_lock<mutex> lock(this->task_mutex);
            task_ = task;
        }
        this->task_condition.notify_one();
    }
};

#endif