#ifndef LOCKFREETASKQUEUE_H
#define LOCKFREETASKQUEUE_H

#include <assert.h>
#include <atomic>
#include <functional>

/**
 * Очередь задач для пула потоков без блокировок
 */
class LockFreeTaskQueue
{
    struct Task
    {
        std::function<void()> task_;
        Task* next_;

        Task(std::function<void()> task) :
            task_(task), next_(nullptr)
        {
        }
    };

    Task* head_;
    std::atomic_flag lockHead_;
    Task* tail_;
    std::atomic_flag lockTail_;

public:
    LockFreeTaskQueue();
    ~LockFreeTaskQueue();

    std::optional<std::function<void()>> acquire();
    bool tryStore(std::function<void()> task);
    void store(std::function<void()> task);
};
#endif // LOCKFREETASKQUEUE_H
