
#include "AbstractStage.hpp"
#include <iostream>

class LastStage : public AbstractStage
{

public:
  using AbstractStage::join;
  using AbstractStage::start;

  LastStage(ThreadSafeQueue<int> *_prevQueue, int thread_id) : AbstractStage(_prevQueue, nullptr, thread_id)
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
        std::cout << readValue << " ";
    }
    std::cout << std::endl;
  }
};
