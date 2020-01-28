#include "MasterWorker.hpp"
#include "WorkerPool.hpp"
#include "DefaultEmitter.hpp"
#include "Timer.hpp"

using namespace std;

//FIXME: refactor
auto activewait = [](int *x) -> int * {
    auto start = std::chrono::high_resolution_clock::now();
    while (true)
    {
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        long long ms = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        if (ms >= *x * 1000)
            break;
    }
    return x;
};

vector<int *> *get_default()
{
    int chunk = 15000;
    vector<int *> *vec = new vector<int *>();

    for (int i = 0; i < chunk; i++)
    {
        vec->push_back(new int(4));
    }

    for (int i = 0; i < chunk; i++)
    {
        vec->push_back(new int(1));
    }

    for (int i = 0; i < chunk; i++)
    {
        vec->push_back(new int(8));
    }

    return vec;
}

vector<int *> *get_constant()
{
    int chunk = 10000;
    vector<int *> *vec = new vector<int *>();

    for (int i = 0; i < chunk; i++)
    {
        vec->push_back(new int(4));
    }

    return vec;
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
    auto emitter = new DefaultEmitter<int>(input_vec);
    auto master = new MasterWorker<int, int>(emitter, nw, activewait, expected_throughput);

    // Timer t("Farm");
    auto results = master
                       ->run()
                       ->get_results();

    // cout << "Count: " << results->size() << endl;
}