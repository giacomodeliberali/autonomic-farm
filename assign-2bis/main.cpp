
#include <iostream>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
#include <ff/farm.hpp>
#include <vector>
#include <chrono>
#include <thread>

using namespace ff;

struct Emitter : ff_node_t<int>
{
  private:
    int length;

  public:
    Emitter(int length) : length(length)
    {
    }

    int *svc(int *)
    {
        for (size_t i = 0; i < length; ++i)
        {
            ff_send_out(new int(i));
        }
        return EOS;
    }
};

struct Worker : ff_node_t<int>
{
    int *svc(int *task)
    {
        int &t = *task;
        t = t * t;
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return task;
    }
};

struct Collector : ff_node_t<int>
{

    Collector(std::vector<int> *output) : output(output)
    {
    }

    int *svc(int *task)
    {
        int &t = *task;
        output->push_back(t);
        //std::cout << "Collector recived: " << t << std::endl;
        return GO_ON;
    }

    std::vector<int> *output;
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " nw length\n";
        return -1;
    }

    int nworkers = std::atoi(argv[1]);
    int length = std::atoi(argv[2]);

    // fed by collector
    std::vector<int> output;

    // set up emittor and collector
    Emitter emitter(length);
    Collector collector(&output);

    // set up workers
    std::vector<std::unique_ptr<ff_node>> W;
    for (int i = 0; i < nworkers; ++i)
        W.push_back(make_unique<Worker>());

    // set up farm
    ff_Farm<int> farm(std::move(W), emitter, collector);

    ffTime(START_TIME);
    farm.run_and_wait_end();
    ffTime(STOP_TIME);

    /*     for (auto &i : output)
        std::cout << "[Farm] Computed: " << i << std::endl; */

    std::cout << "[Farm] Time: " << ffTime(GET_TIME) << "\n";

    // set up parallel for
    ParallelFor pf;
    ffTime(START_TIME);
    pf.parallel_for(0, length, [&](int i) {
        output[i] *= output[i];
    });
    ffTime(STOP_TIME);

    /*     for (int i = 0; i < length; i++)
        std::cout << "[ParallelFor] Computed: " << output[i] << std::endl; */

    std::cout << "[ParallelFor] Time: " << ffTime(GET_TIME) << "\n";

    return 0;
}
