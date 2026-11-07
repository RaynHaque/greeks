#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads) {
  for (size_t i = 0; i < num_threads; ++i) {
    threads_.emplace_back([this] {
      while (true) {
        std:: function < void() > task;
        {
          std::unique_lock < std::mutex > lock(
            queue_mutex_);
          cv_.wait(lock, [this] {
            return !tasks_.empty() || stop_;
          });
          if (stop_ && tasks_.empty()) {
            return;
          }
          task = move(tasks_.front());
          tasks_.pop();
        }

        task();

        {
          std::lock_guard lock(queue_mutex_);
          if (--tasksInProgress_ == 0) {
            allDone_.notify_all();
          };
        }
      }
    });
  }
}
ThreadPool::~ThreadPool() {
  {
    std::unique_lock < std::mutex > lock(queue_mutex_);
    stop_ = true;
  }
  cv_.notify_all();
  for (auto & thread: threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  tasksInProgress_ = 0;
  allDone_.notify_all();

}