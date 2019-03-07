#pragma once

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <chrono>
#include <cstddef>
#include <math.h>
#include <string>
#include <thread>

using namespace std::literals::chrono_literals;

template <typename T>
class ThreadSafeQueue
{
  private:
    std::mutex d_mutex;
    std::condition_variable d_condition;
    std::deque<T> d_queue;

  public:
    ThreadSafeQueue(std::string s) { std::cout << "Created " << s << " queue " << std::endl; }
    ThreadSafeQueue() {}

    void push(T const &value)
    {
        {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            d_queue.push_front(value);
        }
        this->d_condition.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        this->d_condition.wait(lock, [=] { return !this->d_queue.empty(); });
        T rc(std::move(this->d_queue.back()));
        this->d_queue.pop_back();
        return rc;
    }

    bool is_empty()
    {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        return (d_queue.empty());
    }
};