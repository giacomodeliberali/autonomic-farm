// g++ main.cpp -lpthread

#include <iostream>
#include "FirstStage.cpp"
#include "SecondStage.cpp"
#include "ThirdStage.cpp"
#include "FourthStage.cpp"
#include "LastStage.cpp"

using namespace std;

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        cout << "Usage: ./pipeline <number_of_items_to_generate>" << endl;
        return 1;
    }

    // get the number of number the pipeline have to generate and process
    int intArg = atoi(argv[1]);

    // create the queues between stages
    ThreadSafeQueue<int> q1;
    ThreadSafeQueue<int> q2;
    ThreadSafeQueue<int> q3;
    ThreadSafeQueue<int> q4;

    // initialize stages with correct queues
    FirstStage streamStage = FirstStage(&q1, intArg);
    SecondStage incrementStage = SecondStage(&q1, &q2);
    ThirdStage squareStage = ThirdStage(&q2, &q3);
    FourthStage decrementStage = FourthStage(&q3, &q4);
    LastStage printStage = LastStage(&q4);

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

    return 0;
}