
#include "UnaryMathStage.hpp"

class ThirdStage : public UnaryMathStage
{

public:
  using UnaryMathStage::execute;
  using UnaryMathStage::get_thread;
  using UnaryMathStage::start;

  ThirdStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue) : UnaryMathStage(_prevQueue, _nextQueue)
  {
  }

  int compute_operation(int readValue)
  {
    return std::pow(readValue, 2);
  }
};
