#include "lockfreetaskqueue.h"

LockFreeTaskQueue::LockFreeTaskQueue() :
    head_(nullptr), lockHead_(false), tail_(nullptr), lockTail_(false)
{
}

LockFreeTaskQueue::~LockFreeTaskQueue()
{
    while (lockTail_.test_and_set())
        ;
    while (lockHead_.test_and_set())
        ;
    Task* head = head_;
    while (head)
    {
        Task* next = head->next_;
        delete head;
        head = next;
    }
}

std::optional<std::function<void()>> LockFreeTaskQueue::acquire()
{
    if (lockHead_.test_and_set()) // std::memory_order::memory_order_release ?
    {
        return {};
    }
    if (!head_)
    {
        lockHead_.clear(std::memory_order::memory_order_relaxed);
        return {};
    }
    Task* head = head_;
    auto task = head->task_;
    if (head_ == tail_)
    {
        while (lockTail_.test_and_set())
            ;
        if (head_ == tail_)
        {
            head_ = tail_ = nullptr;
        }
        else
        {
            head_ = head_->next_;
            assert(head_);
        }
        lockTail_.clear();
    }
    else
    {
        head_ = head_->next_;
    }
    delete head;
    lockHead_.clear();
    return {task};
}

bool LockFreeTaskQueue::tryStore(std::function<void()> task)
{
    if (lockTail_.test_and_set()) // std::memory_order::memory_order_release ?
    {
        return false;
    }

    if (!tail_)
    {
        assert(!head_);
        while (lockHead_.test_and_set())
            ;
        head_ = tail_ = new Task(task);
        lockHead_.clear();
    }
    else
    {
        tail_ = tail_->next_ = new Task(task);
    }

    lockTail_.clear();
    return true;
}

void LockFreeTaskQueue::store(std::function<void()> task)
{
    while (!tryStore(task))
        ;
}
