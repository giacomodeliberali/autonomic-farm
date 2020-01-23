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
        if (milliseconds >= 10) // 1 sec
            break;
    }

    return x;
};

vector<int *> *getInputVector()
{
    const int SECTION_SIZE = 1000;
    vector<int *> *vec = new vector<int *>();

    cout << "Array: " << SECTION_SIZE << endl;
    for (int i = 1; i <= SECTION_SIZE; i++)
        vec->push_back(new int(i));

    return vec;
}

int main()
{

    int nw = 4;
    cout << "[main.cpp] Initial nw =  " << nw << endl;

    auto start = chrono::high_resolution_clock::now();

    auto emitter = new DefaultEmitter<int>(getInputVector());
    auto master = new MasterWorker<int, int>(emitter, nw, activewait);
    master->start();
    master->join();
    auto elapsed = chrono::high_resolution_clock::now() - start;
    long long milliseconds = chrono::duration_cast<chrono::milliseconds>(elapsed).count();

    cout << "[main.cpp] Ended in " << milliseconds << " ms" << endl;
}