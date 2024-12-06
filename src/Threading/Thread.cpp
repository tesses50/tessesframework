#include "TessesFramework/Threading/Thread.hpp"
#include <iostream>
namespace Tesses::Framework::Threading
{
    #if defined(GEKKO)
    void* Thread::cb(void* data)
    #else
    int Thread::cb(void* data)
    #endif
    {
        auto thrd = static_cast<Thread*>(data);
        
        auto fn = thrd->fn;
        thrd->hasInvoked=true;
        fn();
        #if defined(GEKKO)
            return NULL;
        #else
            return 0;
        #endif
    }
    Thread::Thread(std::function<void()> fn)
    {
        this->hasInvoked=false;
        this->fn = fn;
            
        #if defined(GEKKO)
            thrd = LWP_THREAD_NULL;
            LWP_CreateThread(&thrd, cb, static_cast<void*>(this), NULL, 12000, LWP_PRIO_HIGHEST);
        #else
            thrd_create(&thrd, cb, static_cast<void*>(this));
        #endif
        while(!this->hasInvoked);
    }
    void Thread::Detach()
    {
        #if !defined(GEKKO)
            thrd_detach(thrd);
        #endif
    }

    void Thread::Join()
    {
        #if defined(GEKKO)
        void* res;
        LWP_JoinThread(thrd,&res);
        #else
        int res;
        thrd_join(thrd,&res);
        #endif
        
    }
}