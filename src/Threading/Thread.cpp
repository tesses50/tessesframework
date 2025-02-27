#include "TessesFramework/Threading/Thread.hpp"
#include <iostream>

namespace Tesses::Framework::Threading
{
   #if defined(TESSESFRAMEWORK_ENABLE_THREADING)

    class ThreadHiddenFieldData {
        public:
        #if defined(_WIN32)

        HANDLE thrd;
        DWORD thrdId;
      
        public:
        #elif defined(GEKKO)
        lwp_t thrd;
    
        
        #else
        pthread_t thrd;
        #endif

        
        std::function<void()> fn;

        std::atomic<bool> hasInvoked;
    };
    

    #if defined(_WIN32)
    static  DWORD __stdcall cb(LPVOID data)
    #else
    static void* cb(void* data)
    #endif
    {
        auto thrd = static_cast<ThreadHiddenFieldData*>(data);
        
        auto fn = thrd->fn;
        thrd->hasInvoked=true;
        fn();
        #if !defined(_WIN32)
            return NULL;
        #else
            return 0;
        #endif
    }
    #endif
   
    Thread::Thread(std::function<void()> fn)
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        auto data = this->data.AllocField<ThreadHiddenFieldData>();
        data->hasInvoked=false;
        data->fn = fn;
        #if defined(_WIN32) 
            data->thrd = CreateThread(NULL,0,cb,static_cast<LPVOID>(data), 0, &data->thrdId);
        #elif defined(GEKKO)
            auto res = LWP_CreateThread(&data->thrd, cb, static_cast<void*>(data), nullptr,12000, 98);
        #else
            pthread_create(&data->thrd,NULL,cb,static_cast<void*>(data));
            //thrd_create(&thrd, cb, static_cast<void*>(this));
        #endif
        while(!data->hasInvoked);
        #endif
    }
    void Thread::Detach()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        auto data = this->data.AllocField<ThreadHiddenFieldData*>();
        #if !defined(GEKKO)
            #if defined(_WIN32)
            CloseHandle(data->thrd);
            #else
            pthread_detach(data->thrd);
            #endif
        #endif
        #endif
    }

    void Thread::Join()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        auto data = this->data.AllocField<ThreadHiddenFieldData>();
        #if defined(_WIN32)
            WaitForSingleObject(data->thrd, INFINITE);
        #elif defined(GEKKO)
        void* res;
        LWP_JoinThread(data->thrd,&res);
        #else
        pthread_join(data->thrd,NULL);
        #endif
        #endif
    }
}
