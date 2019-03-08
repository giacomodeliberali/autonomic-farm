
#include "UnaryMathStage.hpp"

class ThirdStage : public UnaryMathStage
{

public:
  using UnaryMathStage::execute;
  using UnaryMathStage::join;
  using UnaryMathStage::start;

  ThirdStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue, int thread_id) : UnaryMathStage(_prevQueue, _nextQueue, thread_id)
  {
  }

  int compute_operation(int readValue)
  {
    return std::pow(readValue, 2);
  }
};
