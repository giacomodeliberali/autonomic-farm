#pragma once
#include <cstdlib>
#include "AbstractStage.hpp"
#include <iostream>
#include "UnaryMathStage.hpp"

class FirstStage : public AbstractStage
{

  private:
    int streamLength;

  public:
    using AbstractStage::join;
    using AbstractStage::start;

    FirstStage(ThreadSafeQueue<int> *_nextQueue, int number, int thread_id) : AbstractStage(nullptr, _nextQueue, thread_id), streamLength(number)
    {
    }

    void execute();
};

class SecondStage : public UnaryMathStage
{

public:
  using UnaryMathStage::execute;
  using UnaryMathStage::join;
  using UnaryMathStage::start;

  SecondStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue, int thread_id) : UnaryMathStage(_prevQueue, _nextQueue, thread_id)
  {
  }

  int compute_operation(int readValue);
};

class ThirdStage : public UnaryMathStage
{

public:
  using UnaryMathStage::execute;
  using UnaryMathStage::join;
  using UnaryMathStage::start;

  ThirdStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue, int thread_id) : UnaryMathStage(_prevQueue, _nextQueue, thread_id)
  {
  }

  int compute_operation(int readValue);
};

class FourthStage : public UnaryMathStage
{

public:
  using UnaryMathStage::execute;
  using UnaryMathStage::join;
  using UnaryMathStage::start;

  FourthStage(ThreadSafeQueue<int> *_prevQueue, ThreadSafeQueue<int> *_nextQueue, int thread_id) : UnaryMathStage(_prevQueue, _nextQueue, thread_id)
  {
  }

  int compute_operation(int readValue);
};

class LastStage : public AbstractStage
{

public:
  using AbstractStage::join;
  using AbstractStage::start;

  LastStage(ThreadSafeQueue<int> *_prevQueue, int thread_id) : AbstractStage(_prevQueue, nullptr, thread_id)
  {
  }

  void execute();
};
