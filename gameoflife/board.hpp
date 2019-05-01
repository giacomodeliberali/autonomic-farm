#pragma once

#include "CImg.h"
#include "shared.hpp"

using namespace cimg_library;

using namespace std;

using board_t = cimg_library::CImg<unsigned char>;

class Board
{
    int n;
    board_t gboard;
    CImgDisplay *matrix_display;

  public:
    Board(int n, CImgDisplay *display) : n(n), matrix_display(display)
    {
        gboard = board_t(n, n, 1, 1, 0);
    }

    void display()
    {
        matrix_display->display(gboard);
        sleep(SLEEP_TIME);
    }

    void init(int seed)
    {
        srand(seed);
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                auto rnd = rand() % 2 == 0 ? ALIVE : DEAD;
                set(i, j, rnd);
            }
        }
    }

    void set(int x, int y, int value)
    {
        gboard(x, y, 0, 0) = value;
    }

    int get(int x, int y)
    {
        return gboard(x, y, 0, 0);
    }

    int get_size()
    {
        return n;
    }
    
    int get_alive_neighbours_count(int x, int y)
    {
        int alive = 0;
        int offsets[] = {-1, 0, 1};

        // search in neighbourgs
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (offsets[i] == 0 && offsets[j] == 0) // same cell, skip
                    continue;

                int x_off = (x + n + offsets[i]) % n; // link bounds
                int y_off = (y + n + offsets[j]) % n;

                if (get(x_off, y_off) == ALIVE)
                    alive++;
            }
        }
        return alive;
    }
};