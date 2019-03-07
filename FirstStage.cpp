
#include <cstdlib>
#include "AbstractStage.hpp"
#include <iostream>

class FirstStage : public AbstractStage
{

private:
  int streamLength;

public:
  using AbstractStage::get_thread;
  using AbstractStage::start;

  FirstStage(ThreadSafeQueue<int> *_nextQueue, int number) : AbstractStage(nullptr, _nextQueue), streamLength(number)
  {
  }

  void execute()
  {
    std::cout << "FirstStage generates: ";
    for (int i = 0; i < streamLength; i++)
    {
      auto computedValue = rand() % 100 + 1;
      nextQueue->push(computedValue);
      std::cout << computedValue << " ";
      std::this_thread::sleep_for(100ms);
    }
    nextQueue->push(0);
    std::cout << std::endl;
    //std::cout << "FirstStage ends " << std::endl;
  }
};
