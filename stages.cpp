
#include "stages.hpp"

void FirstStage::execute()
{
  for (int i = 0; i < streamLength; i++)
  {
    auto computedValue = i + 1; //rand() % 100 + 1;
    nextQueue->push(computedValue);
    //std::cout << "[" << computedValue << "]" << " ";
    std::this_thread::sleep_for(10ms);
  }
  nextQueue->push(0);
  //std::cout << std::endl;
  //std::cout << "FirstStage ends " << std::endl;
}

int SecondStage::compute_operation(int readValue)
{
  return readValue + 1;
}

int ThirdStage::compute_operation(int readValue)
{
  return std::pow(readValue, 2);
}

int FourthStage::compute_operation(int readValue)
{
  return readValue - 1;
}

void LastStage::execute()
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