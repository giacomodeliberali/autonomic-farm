#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <deque>
#include <thread>

using namespace std;
using namespace std::literals::chrono_literals;

template <typename T>
class ThreadSafeQueue
{
private:
    mutex d_mutex;
    condition_variable d_condition;
    deque<T> d_queue;

public:
    ThreadSafeQueue() {}

    void push(T const &value)
    {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        d_queue.push_front(value);
    }

    void notify()
    {
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

    vector<T> pop_all()
    {
        unique_lock<mutex> lock(this->d_mutex);
        vector<T> content;
        this->d_condition.wait(lock, [=] { return !this->d_queue.empty(); });
        int size = d_queue.size();
        for (int i = 0; i < size; i++)
        {
            content.push_back(move(this->d_queue.front()));
            this->d_queue.pop_front();
        }
        return content;
    }

    int size()
    {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        return this->d_queue.size();
    }
};

#endif