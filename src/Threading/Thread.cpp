#include "TessesFramework/Threading/Thread.hpp"
#include <iostream>
namespace Tesses::Framework::Threading
{
    #if defined(_WIN32)
    static  DWORD __stdcall cb(LPVOID data)
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
    #else

   
    void* Thread::cb(void* data)
   {

        auto thrd = static_cast<Thread*>(data);
        
        auto fn = thrd->fn;
        thrd->hasInvoked=true;
        fn();
            return NULL;
        
    }
    #endif
    Thread::Thread(std::function<void()> fn)
    {
        this->hasInvoked=false;
        this->fn = fn;
        #if defined(_WIN32) 
            this->thrd = CreateThread(NULL,0,cb,static_cast<LPVOID>(this), 0, &this->thrdId);
        #elif defined(GEKKO)
            auto res = LWP_CreateThread(&this->thrd, cb, static_cast<void*>(this), nullptr,12000, 98);
        #else
            pthread_create(&thrd,NULL,cb,static_cast<void*>(this));
            //thrd_create(&thrd, cb, static_cast<void*>(this));
        #endif
        while(!this->hasInvoked);
    }
    void Thread::Detach()
    {
        #if !defined(GEKKO)
            #if defined(_WIN32)
            CloseHandle(thrd);
            #else
            pthread_detach(thrd);
            #endif
        #endif
    }

    void Thread::Join()
    {
        #if defined(_WIN32)
            WaitForSingleObject(this->thrd, INFINITE);
        #elif defined(GEKKO)
        void* res;
        LWP_JoinThread(thrd,&res);
        #else
        pthread_join(thrd,NULL);
        #endif
        
    }
}
