#pragma once
#if defined(_WIN32)
#include <windows.h>
#elif defined(GEKKO)
#include <ogc/mutex.h>
#else
#include <threads.h>
#endif
namespace Tesses::Framework::Threading
{
    class Mutex {
        #if defined(_WIN32)
        HANDLE mtx;
        #elif defined(GEKKO)
        mutex_t mtx;
        #else
        mtx_t mtx;
        #endif
        public:
            Mutex();
            void Lock();
            void Unlock();
            bool TryLock();
            ~Mutex();
    };
}