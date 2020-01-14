#include "threadpool.h"

ThreadPool::ThreadPool(std::size_t poolSize): isDestroyed_(false)
{
    for (std::size_t i = 0; i < poolSize; ++i)
    {
        pool_.push_back(std::thread(std::bind(&ThreadPool::runThread, this)));
    }
}

ThreadPool::~ThreadPool()
{
#ifdef LOCK_FREE_QUEUE
    isDestroyed_ = true;
    std::atomic_thread_fence(std::memory_order::memory_order_seq_cst); // без этого изменение isDestroyed_ не было видно в потоках
#else
    {
        std::unique_lock<std::mutex> lock(mutex_);
        isDestroyed_ = true;
    }
#endif
    waiter_.notify_all();
    for(auto& thread : pool_)
    {
        thread.join();
    }
}

#ifdef LOCK_FREE_QUEUE
void ThreadPool::runThread()
{
    while(!isDestroyed_)
    {
        auto task = tasks_.acquire();
        if (!task.has_value())
        {
            std::unique_lock<std::mutex> lock(mutex_);
            waiter_.wait(lock);
        }
        else
        {
            task.value()();
        }
    }
}
#else
void ThreadPool::runThread()
{
    while(!isDestroyed_)
    {
        std::optional<std::function<void()>> task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (tasks_.empty())
            {
                waiter_.wait(lock);
            }
            else
            {
                task.emplace(tasks_.front());
                tasks_.pop();
            }
        }
        if (task.has_value())
        {
            task.value()();
        }
    }
}
#endif
