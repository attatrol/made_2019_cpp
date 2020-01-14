#ifndef THREADPOOL_H
#define THREADPOOL_H

#define LOCK_FREE_QUEUE

#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>

#ifdef LOCK_FREE_QUEUE
#include "lockfreetaskqueue.h"
#else
#include <queue>
#endif

class ThreadPool
{
private:
    bool isDestroyed_;               // флаг завершения работы всех потоков
#ifdef LOCK_FREE_QUEUE
    LockFreeTaskQueue tasks_;        // очередь задач
#else
    std::queue<std::function<void()>> tasks_;
#endif
    std::mutex mutex_;               // мьютекс для организации критических секций
    std::condition_variable waiter_; // объект контроля активности потоков
    std::vector<std::thread> pool_;  // потоки

    void runThread();
public:
    explicit ThreadPool(std::size_t poolSize);
    ~ThreadPool();

    template <class Func, class... Args>
    auto exec(Func func, Args... args) -> std::future<decltype(func(args...))>
    {
        std::packaged_task<decltype(func(args...))()>* task = new std::packaged_task<decltype(func(args...))()>(std::bind(func, args...));
        auto result = task->get_future();
        auto wrapper = [task] ()
        {
            (*task)();
            delete task;
        };
#ifdef LOCK_FREE_QUEUE
        tasks_.store(wrapper);
#else
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push(wrapper);
        }
#endif
        waiter_.notify_all();
        return std::move(result);
    }
    std::size_t size() const
    {
        return pool_.size();
    }
};
#endif // THREADPOOL_H
