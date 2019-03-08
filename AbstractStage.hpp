#pragma once

#include <iostream>
#include "ThreadSafeQueue.cpp"

class AbstractStage
{

private:
  std::thread *thread;

  void exec_in_thread()
  {
    std::cout << "Stage " << thread_id << " on CPU " << sched_getcpu() << std::endl;
    this->execute();
  }

protected:
  ThreadSafeQueue<int> *prevQueue;
  ThreadSafeQueue<int> *nextQueue;
  int thread_id;

public:
  virtual void execute() = 0;

  AbstractStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue, int _thread_id)
      : prevQueue(_prevQueue),
        nextQueue(_nextQueue),
        thread_id(_thread_id)
  {
  }

  void join()
  {
    return thread->join();
  }

  void start()
  {
    thread = new std::thread(&AbstractStage::exec_in_thread, this);

    // stick to cpu

    unsigned num_cpus = std::thread::hardware_concurrency();

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(thread_id % num_cpus, &cpuset);
    int rc = pthread_setaffinity_np(thread->native_handle(),
                                    sizeof(cpu_set_t), &cpuset);

    if (rc != 0)
    {
      std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
    }
  }
};