#pragma once
#include <functional>
#if defined(_WIN32)
#include <windows.h>
#elif defined(GEKKO)
#include <ogc/lwp.h>
#else
#include <threads.h>
#endif
#include <atomic>
namespace Tesses::Framework::Threading
{
    class Thread 
    {
        #if defined(_WIN32)

        HANDLE thrd;
        DWORD thrdId;
      
        public:
        #elif defined(GEKKO)
        lwp_t thrd;
        static void* cb(void* ptr);
        #else
        thrd_t thrd;
        static int cb(void* ptr);
        #endif
        std::function<void()> fn;

        std::atomic<bool> hasInvoked;
        public:
        Thread(std::function<void()> fn);
        void Join();
        void Detach();
    };
}