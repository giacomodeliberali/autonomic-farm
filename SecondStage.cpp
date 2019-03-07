
#include <cstdlib>
#include "UnaryMathStage.hpp"
#include <iostream>

class SecondStage : public UnaryMathStage
{

public:
  using UnaryMathStage::execute;
  using UnaryMathStage::get_thread;
  using UnaryMathStage::start;

  SecondStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue) : UnaryMathStage(_prevQueue, _nextQueue)
  {
  }

  int compute_operation(int readValue)
  {
    return readValue + 1;
  }
};
