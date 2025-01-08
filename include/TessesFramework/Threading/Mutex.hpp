#pragma once
#if defined(_WIN32)
#include <windows.h>
#elif defined(GEKKO)
#include <ogc/mutex.h>
#else
#include <pthread.h>
#endif
namespace Tesses::Framework::Threading
{
    class Mutex {
        #if defined(_WIN32)
        HANDLE mtx;
        #elif defined(GEKKO)
        mutex_t mtx;
        #else
        pthread_mutex_t mtx;
        pthread_mutexattr_t attr;
        #endif
        public:
            Mutex();
            void Lock();
            void Unlock();
            bool TryLock();
            ~Mutex();
    };
}