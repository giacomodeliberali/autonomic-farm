#pragma once

#include <iostream>
#include "ThreadSafeQueue.cpp"

class AbstractStage
{

private:

  std::thread * thread;

protected:
  ThreadSafeQueue<int> *prevQueue;
  ThreadSafeQueue<int> *nextQueue;

public:
  virtual void execute() = 0;

  AbstractStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue)
      : prevQueue(_prevQueue),
        nextQueue(_nextQueue)
  {
  }

 
  void join()
  {
    return thread->join();
  }
  
  void start()
  {
    thread = new std::thread(&AbstractStage::execute, this);
  }


};