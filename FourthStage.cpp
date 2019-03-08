
#include "UnaryMathStage.hpp"

class FourthStage : public UnaryMathStage
{

public:
  using UnaryMathStage::execute;
  using UnaryMathStage::join;
  using UnaryMathStage::start;

  FourthStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue, int thread_id) : UnaryMathStage(_prevQueue, _nextQueue, thread_id)
  {
  }

  int compute_operation(int readValue)
  {
    return readValue - 1;
  }
};
