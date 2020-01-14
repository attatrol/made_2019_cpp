#include <chrono>
#include <ctime>
#include <iostream>
#include <ratio>

#include "threadpool.h"

struct alignas(64) ThreadCompletionMark
{
    std::size_t value;

    ThreadCompletionMark() : value(0)
    {
    }
};

void testThreadpoolSimple(std::size_t poolSize, std::size_t taskCount)
{
    assert(poolSize);
    ThreadPool pool(poolSize);

    std::vector<ThreadCompletionMark> completionMarks(taskCount);
    std::vector<std::future<void>> futures;

    for (std::size_t i = 0; i < taskCount; ++i)
    {
        futures.push_back(pool.exec([&completionMarks, i]() {
           completionMarks[i].value += 1;
        }));
    }

    for (auto& future : futures)
    {
        future.wait();
    }

    std::size_t count = 0;
    for (std::size_t i = 0; i < taskCount; ++i)
    {
        count += completionMarks[i].value;
    }
    assert(count == taskCount /*(taskCount - 2) * (taskCount - 1) / 2*/);
}

void testThreadpoolSaturateQueue(std::size_t poolSize, std::size_t taskCount)
{
    assert(poolSize);
    ThreadPool pool(poolSize);

    std::vector<ThreadCompletionMark> completionMarks(taskCount);
    std::vector<std::future<void>> futures;

    for (std::size_t i = 0; i < taskCount; ++i)
    {
        futures.push_back(pool.exec([&completionMarks, &pool, i]() {
            if (i < pool.size())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
            }
            completionMarks[i].value = i;
        }));
    }

    for (auto& future : futures)
    {
        future.wait();
    }

    std::size_t count = 0;
    for (std::size_t i = 0; i < taskCount; ++i)
    {
        count += completionMarks[i].value;
    }
    assert(count != (taskCount - 2) * (taskCount - 1) / 2);
}

void testThreadpoolRelaxQueue(std::size_t poolSize, std::size_t taskCount)
{
    assert(poolSize);
    ThreadPool pool(poolSize);

    std::vector<ThreadCompletionMark> completionMarks(taskCount);
    std::vector<std::future<void>> futures;

    for (std::size_t i = 0; i < taskCount; ++i)
    {
        futures.push_back(pool.exec([&completionMarks, &pool, i]() {
            completionMarks[i].value = i;
        }));
        if (!(i % (taskCount / 100)))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(3 * pool.size()));
        }
    }

    for (auto& future : futures)
    {
        future.wait();
    }

    std::size_t count = 0;
    for (std::size_t i = 0; i < taskCount; ++i)
    {
        count += completionMarks[i].value;
    }
    assert(count != (taskCount - 2) * (taskCount - 1) / 2);
}

std::size_t calcSum(ThreadPool* pool, std::size_t depth)
{
    if (depth == pool->size() - 1)
    {
        return 1;
    }
    auto future = pool->exec(calcSum, pool, depth + 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(150 * depth));
    future.wait();
    return 1 + future.get();
}

void testThreadpoolRecursion(std::size_t poolSize)
{
    assert(poolSize);
    ThreadPool pool(poolSize);

    auto future = pool.exec(calcSum, &pool, 0);
    future.wait();

    assert(future.get() == pool.size());
}

int main()
{
    // тест из очень коротких тасков, оценка быстродействия
    auto t0 = std::chrono::high_resolution_clock::now();
    testThreadpoolSimple(10,  100  );
    testThreadpoolSimple(100, 1000 );
    testThreadpoolSimple(1,   1000 );
    testThreadpoolSimple(3,   10000);

    // сначала - долгие таски для каждого потока, в это время заполнить очередь короткими тасками
    auto t1 = std::chrono::high_resolution_clock::now();
    testThreadpoolSaturateQueue(10,  100  );
    testThreadpoolSaturateQueue(100, 1000 );
    testThreadpoolSaturateQueue(1,   1000 );
    testThreadpoolSaturateQueue(3,   10000);

    // паузы между заполнением очереди, потоки спят
    auto t2 = std::chrono::high_resolution_clock::now();
    testThreadpoolRelaxQueue(10,  100  );
    testThreadpoolRelaxQueue(100, 1000 );
    testThreadpoolRelaxQueue(1,   1000 );
    testThreadpoolRelaxQueue(3,   10000);

    // таски создаются из самих потоков пула, достигается положение перед дедлоком
    auto t3 = std::chrono::high_resolution_clock::now();
    testThreadpoolRecursion(1);
    testThreadpoolRecursion(10);
    testThreadpoolRecursion(100);

    auto t4 = std::chrono::high_resolution_clock::now();

#ifdef LOCK_FREE_QUEUE
    std::cout << "lock free\n";
#else
    std::cout << "lock\n";
#endif
    std::cout << "testThreadpoolSimple:        " << (std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0)).count() << "s\n" <<
                 "testThreadpoolSaturateQueue: " << (std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1)).count() << "s\n" <<
                 "testThreadpoolRelaxQueue:    " << (std::chrono::duration_cast<std::chrono::duration<double>>(t3 - t2)).count() << "s\n" <<
                 "testThreadpoolRecursion:     " << (std::chrono::duration_cast<std::chrono::duration<double>>(t4 - t3)).count() << "s\n";

//    lock free
//    testThreadpoolSimple:        0.386416s
//    testThreadpoolSaturateQueue: 0.672176s

//    lock
//    testThreadpoolSimple:        0.44621s
//    testThreadpoolSaturateQueue: 0.742427s


    std::cout << "Tests finished" << std::endl;
    return 0;
}
