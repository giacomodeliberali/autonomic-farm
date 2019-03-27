#pragma once
#include <cstdlib>
#include <iostream>
#include "ThreadSafeQueue.cpp"
#include <experimental/optional>

using namespace std;

class Supplier
{

private:
  int stream_length;
  int stream_max;
  ThreadSafeQueue<experimental::optional<int>> *input_queue;
  std::thread *thread;

  void execute()
  {

    for (int i = 0; i < stream_length; i++)
    {
      auto rnd = rand() % stream_max;
      input_queue->push(rnd);
    }
    input_queue->push({});
  }

public:
  Supplier(ThreadSafeQueue<experimental::optional<int>> *_input_queue, int _stream_max, int _stream_length) : stream_length(_stream_length),
                                                                                                              input_queue(_input_queue),
                                                                                                              stream_max(_stream_max)
  {
    srand(time(NULL));
    thread = new std::thread(&Supplier::execute, this);
  }

  void join()
  {
    thread->join();
  }
};

class Worker
{

private:
  ThreadSafeQueue<experimental::optional<int>> *input_queue;
  ThreadSafeQueue<experimental::optional<int>> *output_queue;
  std::thread *thread;
  int worker_id;

  void execute()
  {
    bool eof = false;

    while (!eof && !input_queue->is_empty())
    {
      auto inpute_value = input_queue->pop();

      try
      {
        int value = inpute_value.value();

        this_thread::sleep_for(1ms);

        int output = 0;
        for (int i = 2; i <= value; i++)
          if (isPrime(i))
            output++;
        output_queue->push(output);
      }
      catch (const std::logic_error &e)
      {
        eof = true;
        cout << "[" << worker_id << "] EOF" << endl;
      }
    }
    cout << "[" << worker_id << "] ENDS" << endl;
  }

  bool isPrime(int n)
  {
    if (n <= 1)
      return false;
    if (n <= 3)
      return true;

    if (n % 2 == 0 || n % 3 == 0)
      return false;

    for (int i = 5; i * i <= n; i = i + 6)
      if (n % i == 0 || n % (i + 2) == 0)
        return false;

    return true;
  }

public:
  Worker(ThreadSafeQueue<experimental::optional<int>> *_input_queue,
         ThreadSafeQueue<experimental::optional<int>> *_output_queue,
         int _worker_id) : output_queue(_output_queue),
                           input_queue(_input_queue),
                           worker_id(_worker_id)
  {
    cout << "Created worker " << worker_id << endl;
  }

  void start()
  {
    thread = new std::thread([&] {
      this->execute();
    });
  }

  void join()
  {
    thread->join();
  }
};
