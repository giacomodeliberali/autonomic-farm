
#include <cstdlib>
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
    std::cout << "LastStage: ";
    int readValue = 1;
    do
    {
      std::this_thread::sleep_for(10ms);
      while (!prevQueue->is_empty())
      {
        readValue = prevQueue->pop();

        if (readValue == 0)
          continue;

        std::cout << readValue << " ";

        //std::cout << " --> ThirdStage writes " << computedValue << std::endl;
      }
    } while (readValue > 0);
    std::cout << std::endl;
  }
};
