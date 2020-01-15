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
        long long microseconds = chrono::duration_cast<chrono::microseconds>(elapsed).count();
        if (microseconds >= 1000 * (*x))
            break;
    }
    return x;
};

vector<int *> *getInputVector()
{
    const int SECTION_SIZE = 3000;
    vector<int *> *vec = new vector<int *>();

    for (int i = 0; i < SECTION_SIZE; i++)
    {
        vec->push_back(new int(4));
    }

    return vec;
}

int main()
{

    auto emitter = new DefaultEmitter<int>(getInputVector());
    auto master = new MasterWorker<int, int>(emitter, 1, activewait);
    master->start();
}