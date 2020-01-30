#include <vector>

vector<int *> *get_default()
{
    int chunk = 30000;
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
    int chunk = 90000;
    vector<int *> *vec = new vector<int *>();

    for (int i = 0; i < chunk; i++)
    {
        vec->push_back(new int(4));
    }

    return vec;
}

vector<int *> *get_reverse_default()
{
    int chunk = 30000;
    vector<int *> *vec = new vector<int *>();

    for (int i = 0; i < chunk; i++)
    {
        vec->push_back(new int(8));
    }

    for (int i = 0; i < chunk; i++)
    {
        vec->push_back(new int(1));
    }

    for (int i = 0; i < chunk; i++)
    {
        vec->push_back(new int(4));
    }

    return vec;
}

vector<int *> *get_lowhigh()
{
    int chunk = 45000;
    vector<int *> *vec = new vector<int *>();

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

vector<int *> *get_highlow()
{
    int chunk = 45000;
    vector<int *> *vec = new vector<int *>();

    for (int i = 0; i < chunk; i++)
    {
        vec->push_back(new int(8));
    }

    for (int i = 0; i < chunk; i++)
    {
        vec->push_back(new int(1));
    }

    return vec;
}