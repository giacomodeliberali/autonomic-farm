
#pragma once

#include <cstdlib>
#include "AbstractStage.hpp"
#include <iostream>

class UnaryMathStage : public AbstractStage
{

public:
  using AbstractStage::get_thread;
  using AbstractStage::start;

  UnaryMathStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue) : AbstractStage(_prevQueue, _nextQueue)
  {
  }

  void execute()
  {
    bool eof = false;

    while (!eof)
    {
      int readValue = prevQueue->pop();
      int computedValue = 0;

      std::this_thread::sleep_for(10ms);

      if (readValue == 0)
        eof = true;
      else
        computedValue = compute_operation(readValue);

      nextQueue->push(computedValue);
    }
  }

  virtual int compute_operation(int a) = 0;
};
