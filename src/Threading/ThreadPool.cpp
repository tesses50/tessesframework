#include "TessesFramework/Threading/ThreadPool.hpp"
#if !defined(GEKKO)
#include <thread>
#endif
namespace Tesses::Framework::Threading
{
    size_t ThreadPool::GetNumberOfCores()
    {
        #if defined(GEKKO)
        return 1;
        #else
        return (size_t)std::thread::hardware_concurrency();
        #endif
    }
    ThreadPool::ThreadPool(size_t threads)
    {
        this->isRunning=true;
        for(size_t i = 0; i < threads; i++)
        {
            this->threads.push_back(new Thread([this]()->void{
                while(true) 
                {
                    this->mtx.Lock();
                
                    if(!this->isRunning) 
                    {
                        this->mtx.Unlock();
                        return;
                    }

                    std::function<void()> fn=nullptr;

                    if(this->callbacks.empty())
                    {
                        fn=this->callbacks.front();
                        this->callbacks.pop();
                    }
                    this->mtx.Unlock();
                    if(fn)
                        fn();
                }
            }));
        }
    }
    void ThreadPool::Schedule(std::function<void()> cb)
    {
        this->mtx.Lock();
        this->callbacks.push(cb);
        this->mtx.Unlock();
    }
    ThreadPool::~ThreadPool()
    {
        while(true)
        {
            this->mtx.Lock();
            auto emp=this->callbacks.empty();
            if(emp) this->isRunning=false;
            this->mtx.Unlock();
            if(emp) break;
        }

        for(auto item : this->threads)
        {
            item->Join();
            delete item;
        }

    }
}