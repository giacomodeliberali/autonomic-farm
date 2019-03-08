
#include "UnaryMathStage.hpp"

class SecondStage : public UnaryMathStage
{

public:
  using UnaryMathStage::execute;
  using UnaryMathStage::join;
  using UnaryMathStage::start;

  SecondStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue, int thread_id) : UnaryMathStage(_prevQueue, _nextQueue, thread_id)
  {
  }

  int compute_operation(int readValue)
  {
    return readValue + 1;
  }
};
