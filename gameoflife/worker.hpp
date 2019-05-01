#include <iostream>
#include "shared.hpp"
#include "board.hpp"
#include <thread>

using namespace std;

class Worker
{

  private:
    thread *tid;
    Board *source;
    Board *target;
    int start;
    int limit;

  public:
    Worker(Board *source, Board *target) : source(source), target(target)
    {
    }

    void set_chunk(int start, int limit)
    {
        this->start = start;
        this->limit = limit;
    }

    void run()
    {
        tid = new thread([&] {
            int n = source->get_size();
            for (int i = this->start; i < limit; i++)
            {
                int x = i % n; // ensure bounds
                int y = (i - x) / n;

                int alive = source->get_alive_neighbours_count(x, y);

                auto cell = source->get(x, y);

                if (alive < 2 || alive > 3)
                    target->set(x, y, DEAD); // dead

                if (alive == 2 || alive == 3)
                    target->set(x, y, cell); // remain the same

                if (alive == 3 && cell != ALIVE)
                    target->set(x, y, ALIVE); // new born
            }
        });
    }

    void join()
    {
        tid->join();
    }
};