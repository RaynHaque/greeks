#pragma once
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool
{
public:
    ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
    ~ThreadPool();
    static void enqueue(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            tasksInProgress_++;
            tasks_.emplace(move(task));
        }
        cv_.notify_one();
    }

    static inline void waitAll()
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        allDone_.wait(lock, [] { return tasksInProgress_ == 0; });
    }

private:
    static inline std::vector<std::jthread> threads_;
    static inline std::queue<std::function<void()>> tasks_;
    static inline std::mutex queue_mutex_;
    static inline std::condition_variable cv_;
    static inline std::condition_variable allDone_;
    static inline int tasksInProgress_ = 0;
    static inline bool stop_;
};