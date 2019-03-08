
#include <cstdlib>
#include "AbstractStage.hpp"
#include <iostream>

class FirstStage : public AbstractStage
{

private:
  int streamLength;

public:
  using AbstractStage::join;
  using AbstractStage::start;

  FirstStage(ThreadSafeQueue<int> *_nextQueue, int number) : AbstractStage(nullptr, _nextQueue), streamLength(number)
  {
  }

  void execute()
  {
    for (int i = 0; i < streamLength; i++)
    {
      auto computedValue = i + 1;//rand() % 100 + 1;
      nextQueue->push(computedValue);
      //std::cout << "[" << computedValue << "]" << " ";
      std::this_thread::sleep_for(10ms);
    }
    nextQueue->push(0);
    //std::cout << std::endl;
    //std::cout << "FirstStage ends " << std::endl;
  }
};
