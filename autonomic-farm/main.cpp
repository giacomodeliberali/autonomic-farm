#include "MasterWorker.hpp"
#include "WorkerPool.hpp"
#include "DefaultEmitter.hpp"
using namespace std;

//FIXME: refactor
auto activewait = [](int *x) -> int * {
    auto start = chrono::high_resolution_clock::now();
    while (true)
    {
        auto elapsed = chrono::high_resolution_clock::now() - start;
        long long milliseconds = chrono::duration_cast<chrono::milliseconds>(elapsed).count();
        if (milliseconds >= 1000 * (*x)) // 1 sec
            break;
    }
    return x;
};

vector<int *> *getInputVector()
{
    const int SECTION_SIZE = 10;
    vector<int *> *vec = new vector<int *>();

    for (int i = 0; i < SECTION_SIZE; i++)
    {
        vec->push_back(new int(1));
    }

    return vec;
}

int main()
{

    auto emitter = new DefaultEmitter<int>(getInputVector());
    auto master = new MasterWorker<int, int>(emitter, 1, activewait);
    master->start();
    master->join();
    cout << "[main.cpp]::end" << endl;
}