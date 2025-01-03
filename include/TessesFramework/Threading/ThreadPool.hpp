#pragma once
#include <functional>
#include "Thread.hpp"
#include "Mutex.hpp"
#include <vector>
#include <queue>

namespace Tesses::Framework::Threading
{
   
class ThreadPool 
{
    std::vector<Thread*> threads;
    std::queue<std::function<void()>> callbacks;
    Mutex mtx;
    volatile bool isRunning;
    public:
        static size_t GetNumberOfCores();
        ThreadPool(size_t threads);
        void Schedule(std::function<void()> cb);
        ~ThreadPool();
};
}