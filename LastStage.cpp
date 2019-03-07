
#include "AbstractStage.hpp"
#include <iostream>

class LastStage : public AbstractStage
{

public:
  using AbstractStage::get_thread;
  using AbstractStage::start;

  LastStage(ThreadSafeQueue<int> *_prevQueue) : AbstractStage(_prevQueue, nullptr)
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
      std::cout << std::endl;
    }
  }
};
