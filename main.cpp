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

    if(argc != 2){
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
    std::vector<std::thread *> tid;
    tid.resize(0);

    // starts all the stages threads
    streamStage.start();
    incrementStage.start();
    squareStage.start();
    decrementStage.start();
    printStage.start();

    // take a reference to each thread
    tid.push_back(streamStage.get_thread());
    tid.push_back(incrementStage.get_thread());
    tid.push_back(squareStage.get_thread());
    tid.push_back(decrementStage.get_thread());
    tid.push_back(printStage.get_thread());

    // join threads and return
    for (auto &t : tid)
    {
        t->join();
    }

    return 0;
}