#ifndef DEFAULT_WORKER_HPP
#define DEFAULT_WORKER_HPP

#include "IFreezableWorker.hpp"
#include "WorkerPool.hpp"
#include <vector>
#include <iostream>
#define END_OF_STREAM 0xffff

using namespace std;

template <typename TIN, typename TOUT>
class DefaultWorker : public IFreezableWorker
{

private:
    void wait_for_task()
    {
        unique_lock<mutex> lock(this->task_mutex);
        if (task_ == nullptr)
        {
            //cout << "\t\t[Worker " << id_ << "] waiting a task " << endl;
            this->task_condition.wait(lock, [&] { return this->task_ != nullptr; });
        }
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
                //cout << "\t\t[Worker] accept EOF " << endl;
                eos = true;
                //cout << "\t\t[Worker] received END_OF_STREAM. Terminating..." << endl;
                pool_->collect(this, (TOUT *)END_OF_STREAM);
                continue;
            }
            //cout << "\t\t[Worker " << id_ << "] compute task " << *task_ << endl;

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
        //if (task != (TIN *)END_OF_STREAM)
        //cout << "\t\t[Worker " << id_ << "] accept task " << *task << endl;
        unique_lock<mutex> lock(this->task_mutex);
        task_ = task;
        this->task_condition.notify_one();
    }
};

#endif