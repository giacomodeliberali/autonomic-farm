
#include <iostream>
#include <vector>
#include "MapReduce.hpp"

using namespace std;

int main(int argc, char *argv[])
{

    auto mapReduce = MapReduce<int, int, int>();
    mapReduce.set_map([](auto input) {
        return new pair(
            input * 2, // double the number
            input % 2  // even and odd numbers
        );
    });
    mapReduce.set_reduce([](auto left, auto right) {
        return left + right; // sum the event and odd numbers
    });

    mapReduce.set_hash([](auto key) {
        return key;
    });

    mapReduce.set_nw(10, 4);

    vector<int> vec;
    for (int i = 1; i <= 100; i++)
        vec.push_back(i);

    for (auto val : mapReduce.run(vec))
    {
        cout << "[Key: " << val->second << "] = " << val->first << endl;
    }
}