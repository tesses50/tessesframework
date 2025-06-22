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
        #elif defined(TESSESFRAMEWORK_ENABLE_THREADING)
        return (size_t)std::thread::hardware_concurrency();
        #else
        return 1;
        #endif
    }
    size_t ThreadPool::ThreadCount()
    {
        return this->threads.size();
    }
    bool ThreadPool::Empty()
    {
        bool qie;
        this->mtx.Lock();
        qie = this->callbacks.empty();
        this->mtx.Unlock();
        return qie;
    }
    ThreadPool::ThreadPool(size_t threads)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        this->isRunning=true;
        for(size_t i = 0; i < threads; i++)
        {
            this->threads.push_back(new Thread([this,i]()->void{
                while(true) 
                {
                    this->mtx.Lock();
                
                    if(!this->isRunning) 
                    {
                        this->mtx.Unlock();
                        return;
                    }

                    std::function<void(size_t)> fn=nullptr;

                    if(!this->callbacks.empty())
                    {
                        fn=this->callbacks.front();
                        this->callbacks.pop();
                    }
                    this->mtx.Unlock();
                    if(fn)
                        fn(i);
                }
            }));
        }
        #endif
    }
    void ThreadPool::Schedule(std::function<void(size_t)> cb)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        this->mtx.Lock();
        this->callbacks.push(cb);
        this->mtx.Unlock();
        #endif
    }
    ThreadPool::~ThreadPool()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
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
        #endif
    }
}