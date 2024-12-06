#pragma once
#include <functional>
#if defined(GEKKO)
#include <ogc/lwp.h>
#else
#include <threads.h>
#endif
#include <atomic>
namespace Tesses::Framework::Threading
{
    class Thread 
    {
        std::atomic<bool> hasInvoked;
        #if defined(GEKKO)
        lwp_t thrd;
        static void* cb(void* ptr);
        #else
        thrd_t thrd;
        static int cb(void* ptr);
        #endif
        std::function<void()> fn;
        public:
        Thread(std::function<void()> fn);
        void Join();
        void Detach();
    };
}