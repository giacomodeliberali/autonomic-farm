#ifndef MASTER_WORKER_HPP
#define MASTER_WORKER_HPP

#include "IFreezableWorker.hpp"
#include "DefaultWorker.hpp"
#include "IEmitter.hpp"
#include "WorkerPool.hpp"
#include <vector>
#include <iostream>

using namespace std;

template <typename TIN, typename TOUT>
class MasterWorker : public IFreezableWorker
{
private:
    IEmitter<TIN> *emitter_;
    WorkerPool<TIN, TOUT> *pool_;

protected:
    // The code to be executed from the master worker in its own thread
    void run() override
    {

        cout << "[Master] run()" << endl;

        int taskNumber = 0;
        int collectedTasks = 0;
        int prevCollectedTask = 0;

        auto next = emitter_->get_next();
        while (next != NULL)
        {
            cout << "NEXT => " << *next << endl;
            if (this->pool_->has_available_worker())
            {
                this->pool_->assign(next);
                collectedTasks++;
                next = emitter_->get_next();
            }
        }

        cout << "[Master] join all" << endl;
        pool_->join_all();
    };

public:
    MasterWorker(IEmitter<TIN> *emitter, int nw, function<TOUT *(TIN *)> func) : emitter_(emitter)
    {
        cout << "[Master] Created" << endl;
        pool_ = new WorkerPool(this, nw, func);
    }

    void collect(TOUT *result)
    {
        // notify monitor
        cout << "Master collected: " << *result << endl;
    }
};

#endif