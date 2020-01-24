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
    vector<T> vector_;

public:
    ThreadSafeQueue() {}

    void push(T const &value)
    {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        vector_.push_back(value);
    }

    // Notify that the size of the queue has changed
    void notify()
    {
        this->d_condition.notify_one();
    }

    T pop()
    {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        this->d_condition.wait(lock, [=] { return !this->vector_.empty(); });
        T rc(std::move(this->vector_.back()));
        this->vector_.pop_back();
        return rc;
    }

    bool is_empty()
    {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        return (vector_.empty());
    }

    vector<T> pop_all()
    {
        unique_lock<mutex> lock(this->d_mutex);
        vector<T> content;
        this->d_condition.wait(lock, [=] { return !this->vector_.empty(); });
        int size = vector_.size();
        for (int i = 0; i < size; i++)
        {
            content.push_back(move(this->vector_.front()));
            this->vector_.pop_back();
        }
        return content;
    }

    int size()
    {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        int size = this->vector_.size();
        return size;
    }
};

#endif