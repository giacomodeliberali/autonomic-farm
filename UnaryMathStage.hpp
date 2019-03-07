
#pragma once

#include <cstdlib>
#include "AbstractStage.hpp"
#include <iostream>

class UnaryMathStage : AbstractStage
{

public:
  using AbstractStage::get_thread;
  using AbstractStage::start;

  UnaryMathStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue) : AbstractStage(_prevQueue, _nextQueue)
  {
  }

  void execute()
  {
    int readValue = 1;
    do
    {
      while (!prevQueue->is_empty())
      {
        readValue = prevQueue->pop();

        std::this_thread::sleep_for(10ms);

        if (readValue == 0)
          continue;

        //std::cout << "ThirdStage reads " << readValue << std::endl;
        auto computedValue = compute_operation(readValue);
        nextQueue->push(computedValue);

        //std::cout << " --> ThirdStage writes " << computedValue << std::endl;
      }
    } while (readValue > 0);
    nextQueue->push(0);
    //std::cout << "ThirdStage end" << std::endl;
  }

  virtual int compute_operation(int a) = 0;
};
