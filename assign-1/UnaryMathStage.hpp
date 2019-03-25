
#pragma once

#include "AbstractStage.hpp"

class UnaryMathStage : public AbstractStage
{

public:
  using AbstractStage::join;
  using AbstractStage::start;

  UnaryMathStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue, int thread_id) : AbstractStage(_prevQueue, _nextQueue, thread_id)
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