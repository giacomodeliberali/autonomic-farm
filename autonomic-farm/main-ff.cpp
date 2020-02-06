#include <iostream>
#include <cassert>
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include "src/emitter/DefaultEmitter.hpp"
#include "src/collector/Collector.hpp"
#include "src/common/inputs.hpp"
#include "src/common/InputType.hpp"
#include "src/common/Flags.hpp"
#include "src/master/IPool.hpp"
#include "src/monitor/Monitor.hpp"

using namespace std;
using namespace ff;

int *func(int *x)
{
    auto start = chrono::high_resolution_clock::now();
    auto end = false;
    while (!end)
    {
        auto elapsed = chrono::high_resolution_clock::now() - start;
        auto ms = chrono::duration_cast<chrono::microseconds>(elapsed).count();
        if (ms >= *x * 1000) // x milliseconds
            end = true;
    }
    return x;
}

template <typename TIN, typename TOUT>
class DefaultFFWorker : public ff_node_t<TIN, TOUT>
{
private:
    function<TOUT *(TIN *)> func_;

public:
    DefaultFFWorker(function<TOUT *(TIN *)> func) : func_(func)
    {
    }

    TOUT *svc(TIN *task)
    {
        if (task == (TIN *)END_OF_STREAM)
            return this->EOS;
        auto result = func_(task);
        this->ff_send_out(result);
        return this->GO_ON;
    }
};

template <typename TIN, typename TOUT>
class MasterFFWorker : public IPool, public ff_monode_t<TOUT, TIN>
{
private:
    IEmitter<TIN> *emitter_;
    Collector<TOUT> *collector_;
    Monitor *monitor_;
    ff_farm *farm_;

    // The actual number of active workers
    int actual_nw_;

    // The max number of workers
    int max_nw_;

    // The difference between emitted and collected task. If 0 means that all the emitted tasks have been also collected.
    int task_diff_ = 0;

public:
    MasterFFWorker(IEmitter<TIN> *emitter, int max_nw, function<TOUT *(TIN *)> func, float expected_throughput) : emitter_(emitter), max_nw_(max_nw)
    {
        // The monitor notifies back to the master since it is his pool reference
        monitor_ = new Monitor(this, expected_throughput);
        collector_ = new Collector<TOUT>();

        vector<ff_node *> workers;
        for (auto i = 0; i < max_nw_; i++)
            workers.push_back(new DefaultFFWorker<TIN, TOUT>(func));

        // the master acts as emitter and collector
        this->farm_ = new ff_farm(workers);
        this->farm_->remove_collector();
        this->farm_->add_emitter(this);
        this->farm_->wrap_around();
    }

    int get_actual_workers_number() override
    {
        return actual_nw_;
    }

    void notify_command(int command) override
    {
        if (FlagUtils::is(command, ADD_WORKER))
        {
            if (get_actual_workers_number() < max_nw_)
            {
                this->thaw(actual_nw_, true); // unfreeze last
                actual_nw_++;
            }
        }
        if (FlagUtils::is(command, REMOVE_WORKER))
        {
            if (get_actual_workers_number() > 1)
            {
                actual_nw_--;
                this->ff_send_out_to(this->GO_OUT, actual_nw_); // freeze last worker
            }
        }
    }

    int svc_init() override
    {
        actual_nw_ = this->get_num_outchannels();
        return 0;
    }

    TIN *svc(TOUT *result) override
    {
        int worker_id = this->get_channel_id();

        if (worker_id < 0)
        {
            // initial scheduling
            monitor_->init();
            for (int worker_id = 0; worker_id < max_nw_; worker_id++)
            {
                auto next = emitter_->get_next();
                task_diff_++;
                this->ff_send_out_to(next, worker_id);
            }
            return this->GO_ON;
        }
        // each time a task gets collected
        if (result)
        {
            monitor_->notify();
            collector_->collect(result);
            task_diff_--;
        }

        auto next = emitter_->get_next();

        if (next)
        {
            task_diff_++;
            this->ff_send_out_to(next, worker_id);
            return this->GO_ON;
        }
        else
        {
            // emitter has ended

            if (task_diff_ != 0)
                return this->GO_ON;

            for (auto i = this->actual_nw_ - 1; i < this->max_nw_ - 1; i++)
            {
                // unfreeze all remaining
                this->thaw(i, true);
                this->actual_nw_++;
            }

            assert(actual_nw_ == max_nw_);

            for (auto i = 0; i < this->actual_nw_; i++)
                this->ff_send_out_to((TIN *)END_OF_STREAM, worker_id);

            return this->EOS;
        }
    }

    MasterFFWorker<TIN, TOUT> *run()
    {
        // http://calvados.di.unipi.it/storage/tutorial/html/tutorial.html
        int run_result = farm_->run_then_freeze(); // run workers and once they are done, freeze them instead of killing them
        assert(run_result == 0);
        farm_->wait_freezing(); // wait all to freeze
        return this;
    }

    vector<TOUT *> *get_results()
    {
        return collector_->get_results();
    }
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cout << "\nUsage is " << argv[0] << " nw throughput [inputType]\n\n";
        cout << "Input type could be: " << endl;
        cout << "\t- 1 = 4L 1L 8L \t(default)" << endl;
        cout << "\t- 2 = 4L 4L 4L \t(constant)" << endl;
        cout << "\t- 3 = 8L 1L 4L \t(reverse default)" << endl;
        cout << "\t- 4 = 1L 8L \t(low high)" << endl;
        cout << "\t- 5 = 8L 1L \t(high low)" << endl;
        return 0;
    }

    int nw = atoi(argv[1]);
    float expected_throughput = atof(argv[2]);

    auto input_vec = get_default();

    if (argc == 4)
    {
        // input provided
        int input_type = atoi(argv[3]);
        switch (input_type)
        {
        case InputType::Constant:
            input_vec = get_constant();
            break;
        case InputType::ReverseDefault:
            input_vec = get_reverse_default();
            break;
        case InputType::LowHigh:
            input_vec = get_lowhigh();
            break;
        case InputType::HighLow:
            input_vec = get_highlow();
            break;
        }
    }

    auto emitter = new DefaultEmitter<int>(input_vec);
    auto master = new MasterFFWorker<int, int>(emitter, nw, &func, expected_throughput);

    auto results = master
                       ->run()
                       ->get_results();
}