#include <iostream>
#include <vector>
#include <cmath>
#include "board.hpp"
#include <chrono>
#include <thread>

using namespace std;

void worker(Board *origin, Board *target, int originY, int limit)
{
    int n = origin->get_size();
    for (int i = originY; i < limit; i++)
    {
        int x = i % n; // ensure bounds
        int y = (i - x) / n; 
        
        int alive = origin->get_alive_neighbours_count(x, y);

        auto cell = origin->get(x, y);

        if (alive < 2 || alive > 3) 
            target->set(x, y, DEAD); // dead

        if (alive == 2 || alive == 3)
            target->set(x, y, cell); // remain the same

        if (alive == 3 && cell != ALIVE)
            target->set(x, y, ALIVE); // new born
    }
}

void make_run(Board *origin, Board *target, int nw)
{
    int n = origin->get_size();
    int dim = n * n;

    // number of items each worker has to compute
    int worker_items = (int)(dim / nw);

    int originY = 0;
    int limit = originY + worker_items;

    vector<thread> pool;
    
    for (int i = 0; i < nw; i++)
    {
        pool.push_back(thread(worker, origin, target, originY, limit));

        originY = limit + 1;

        if (originY >= dim) // the matrix is complete
            break;

        limit = originY + worker_items; // next chunk
    }

    for (auto &t : pool)
        t.join();
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

    // build 2 matrixes with same display
    Board *origin_board = new Board(n, main_displ);
    Board *target_board = new Board(n, main_displ);

    // initialize
    origin_board->init(seed);
    origin_board->display();

    for (int i = 0; i < iterations; i++)
    {
        make_run(origin_board, target_board, nw);

        // display result
        target_board->display();

        // swap pointers
        Board *tmp = origin_board;
        origin_board = target_board;
        target_board = tmp;
    }

    return 0;
}