// g++ main.cpp -lpthread

#include <iostream>
#include "stages.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: ./pipeline <number_of_items_to_generate>" << endl;
        return 1;
    }

    chrono::system_clock::time_point start = std::chrono::system_clock::now();

    // get the number of number the pipeline have to generate and process
    int intArg = atoi(argv[1]);

    // create the queues between stages
    ThreadSafeQueue<int> q1;
    ThreadSafeQueue<int> q2;
    ThreadSafeQueue<int> q3;
    ThreadSafeQueue<int> q4;

    // initialize stages with correct queues
    FirstStage streamStage = FirstStage(&q1, intArg, 1);
    SecondStage incrementStage = SecondStage(&q1, &q2, 2);
    ThirdStage squareStage = ThirdStage(&q2, &q3, 3);
    FourthStage decrementStage = FourthStage(&q3, &q4, 4);
    LastStage printStage = LastStage(&q4, 5);

    // keep track of all threads
    std::vector<AbstractStage *> stages;
    stages.resize(0);

    stages.push_back(&streamStage);
    stages.push_back(&incrementStage);
    stages.push_back(&squareStage);
    stages.push_back(&decrementStage);
    stages.push_back(&printStage);

    // join threads and return
    for (auto &stage : stages)
        stage->start();

    for (auto &stage : stages)
        stage->join();

    chrono::system_clock::time_point end = std::chrono::system_clock::now();
    cout << "Execution time: " << chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;

    return 0;
}