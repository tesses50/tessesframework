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
    std::queue<std::function<void(size_t)>> callbacks;
    Mutex mtx;
    volatile bool isRunning;
    public:
        static size_t GetNumberOfCores();
        ThreadPool(size_t threads);
        size_t ThreadCount();
        bool Empty();
        void Schedule(std::function<void(size_t)> cb);
        ~ThreadPool();
};
}