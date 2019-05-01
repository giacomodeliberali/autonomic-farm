#include <iostream>
#include <vector>
#include <cmath>
#include "board.hpp"
#include "worker.hpp"
#include <chrono>
#include <thread>

using namespace std;

void make_run(Board *source, Board *target, int nw)
{
    int n = source->get_size();
    int dim = n * n;

    // number of items each worker has to compute
    int worker_items = (int)(dim / nw);

    int start = 0;
    int limit = start + worker_items;

    vector<Worker*> workers;

    for (int i = 0; i < nw; i++)
    {
        auto w = new Worker(source, target);
        w->set_chunk(start, limit);
        w->run();

        workers.push_back(w);

        start = limit + 1;

        if (start >= dim) // the matrix is complete
            break;

        limit = start + worker_items; // next chunk
    }

    for (auto &w : workers){
        w->join();
        delete w;
    }
}

int main(int argc, char *argv[])
{
    // nice result => ./main 500 10000 8
    if (argc < 4)
    {
        cout << "./gameoflife <n> <iterations> <nw> [seed]" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    int iterations = atoi(argv[2]);
    int nw = atoi(argv[3]);
    int seed = chrono::system_clock::now().time_since_epoch().count();
    if (argc == 5)
        seed = atoi(argv[4]);

    // the board display
    CImgDisplay *main_displ = new CImgDisplay();
    main_displ->set_title("Game of Life");

    // build 2 matrixes with same display
    Board *source = new Board(n, main_displ);
    Board *target = new Board(n, main_displ);

    // initialize
    source->init(seed);
    source->display();

    for (int i = 0; i < iterations; i++)
    {
        make_run(source, target, nw);

        // display result
        target->display();

        // swap pointers
        Board *tmp = source;
        source = target;
        target = tmp;
    }

    return 0;
}