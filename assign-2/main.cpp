// g++ main.cpp -lpthread

#include <iostream>
#include "stages.cpp"
#include <experimental/optional>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: ./pipeline <nw>" << endl;
        return 1;
    }

    // get the number of workers
    int nw = atoi(argv[1]);

    std::cout << "nw = " << nw << std::endl;

    vector<Worker *> workers;
    workers.resize(0);

    // create the queues between stages
    ThreadSafeQueue<experimental::optional<int>> input_queue;
    ThreadSafeQueue<experimental::optional<int>> output_queue;

    // put in input_queue 1000 numbers in range [0,10000]
    Supplier supplier = Supplier(&input_queue, 10000, 10000);

    // join supplier
    supplier.join();

    // set up workers
    for (int i = 0; i < nw; i++)
    {
        Worker *w_i = new Worker(&input_queue, &output_queue, i);
        workers.push_back(w_i);
    }

    chrono::system_clock::time_point start = std::chrono::system_clock::now();

    for (auto w_i : workers)
        w_i->start();

    for (auto w_i : workers)
        w_i->join();

    chrono::system_clock::time_point end = std::chrono::system_clock::now();
    std::cout << "Execution time: " << chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << std::endl;

    return 0;
}