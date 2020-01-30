#include "MasterWorker.hpp"
#include "WorkerPool.hpp"
#include "DefaultEmitter.hpp"
#include "Timer.hpp"
#include "inputs.hpp"
#include "InputType.hpp"

using namespace std;

int *func(int *x)
{
    auto start = chrono::high_resolution_clock::now();
    while (true)
    {
        auto elapsed = chrono::high_resolution_clock::now() - start;
        long long ms = chrono::duration_cast<chrono::microseconds>(elapsed).count();
        if (ms >= *x * 1000)
            break;
    }
    return x;
}

int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        cout << "Usage is: [nw] [throughput] " << endl;
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
        default:
            input_vec = get_default();
            break;
        }
    }

    auto emitter = new DefaultEmitter<int>(input_vec);
    auto master = new MasterWorker<int, int>(emitter, nw, &func, expected_throughput);

    // Timer t("Farm");
    auto results = master
                       ->run()
                       ->get_results();

    // cout << "Count: " << results->size() << endl;
}