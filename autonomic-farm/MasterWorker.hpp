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
    void run() override{
        auto next = emitter_->get_next();
        while (next != nullptr)
        {
            this->pool_->assign(next);
            next = emitter_->get_next();
        }

        auto joined_workers = pool_->join_all();
        
        cout << "[Master] joined all (" << joined_workers << "). Sum = " << sum << endl;
    };

public:
    MasterWorker(IEmitter<TIN> *emitter, int nw, function<TOUT *(TIN *)> func) : emitter_(emitter)
    {
        pool_ = new WorkerPool(this, nw, func);
    }

    int sum = 0;
    void collect(TOUT *result)
    {
        // notify monitor
        sum += *result;
        //cout << "Master collected: " << *result << endl;
    }
};

#endif