
#include <iostream>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
#include <ff/farm.hpp>
#include <vector>
#include <chrono>
#include <thread>

using namespace ff;

bool is_prime(int n);
int count_prime_numbers_up_to(int n);

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
        for (int i = 0; i < length; ++i)
        {
            ff_send_out(new int(i));
        }
        return EOS;
    }
};

struct Worker : ff_node_t<int, std::pair<int, int>>
{
    std::pair<int, int> *svc(int *number)
    {
        return new std::pair<int, int>(*number, count_prime_numbers_up_to(*number));
    }
};

struct Collector : ff_node_t<std::pair<int, int>, int>
{

    std::vector<std::pair<int, int> *> *output;

    Collector(std::vector<std::pair<int, int> *> *output) : output(output)
    {
    }

    int *svc(std::pair<int, int> *pair)
    {
        output->push_back(pair);
        //std::cout << "Collector recived: " << pair->first << "->" << pair->second << std::endl;
        return GO_ON;
    }
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " nw length\n";
        return -1;
    }

    int nw = std::atoi(argv[1]);
    int length = std::atoi(argv[2]);

    // fed by collector
    std::vector<std::pair<int, int> *> output;
    output.resize(0);

    // set up emittor and collector
    Emitter emitter(length);
    Collector collector(&output);

    // set up workers
    std::vector<std::unique_ptr<ff_node>> W;
    for (int i = 0; i < nw; ++i)
        W.push_back(make_unique<Worker>());

    // set up farm
    ff_Farm<int> farm(std::move(W), emitter, collector);

    ffTime(START_TIME);
    farm.run_and_wait_end();
    ffTime(STOP_TIME);

    /*     for (auto &i : output)
        std::cout << "[Farm] Computed: " << i << std::endl; */

    std::cout << "[Farm, nw=" << nw << "] Time: " << ffTime(GET_TIME) << "\n";

    // set up parallel for
    ParallelFor pf;
    ffTime(START_TIME);
    pf.parallel_for(0, length, nw, [&](int i) {
        //std::cout << "PF: " << output.at(i)->first << "->" << output.at(i)->second << std::endl;
        output.at(i)->second = count_prime_numbers_up_to(output.at(i)->first);
    });
    ffTime(STOP_TIME);

    /*     for (int i = 0; i < length; i++)
        std::cout << "[ParallelFor] Computed: " << output[i] << std::endl; */

    std::cout << "[ParallelFor, nw=" << nw << "] Time: " << ffTime(GET_TIME) << "\n";

    return 0;
}

bool is_prime(int n)
{
    for (int i = 2; i < sqrt(n); i++)
    {
        if (n % i == 0)
            return false;
    }
    return true;
}

int count_prime_numbers_up_to(int n)
{
    int prime_numbers = 0;

    for (int i = 2; i < n; i++)
        if (is_prime(i))
            prime_numbers++;

    return prime_numbers;
}