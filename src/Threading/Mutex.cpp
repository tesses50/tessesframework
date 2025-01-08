#include "TessesFramework/Threading/Mutex.hpp"
#include <cstring>
#include <iostream>
namespace Tesses::Framework::Threading
{
    Mutex::Mutex()
    {
        #if defined(_WIN32)
        this->mtx = CreateMutex(NULL,false,NULL);
        #elif defined(GEKKO)
        mtx = LWP_MUTEX_NULL;
        LWP_MutexInit(&mtx, true);
        #else
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mtx,&attr);
    
        #endif
    }
    void Mutex::Lock()
    {
        #if defined(_WIN32)
        WaitForSingleObject(mtx, INFINITE);
        #elif defined(GEKKO)
        LWP_MutexLock(mtx);
        #else
        pthread_mutex_lock(&mtx);
        #endif
    }
    void Mutex::Unlock()
    {
        #if defined(_WIN32)
        ReleaseMutex(mtx);
        #elif defined(GEKKO)
        LWP_MutexUnlock(mtx);
        #else
        pthread_mutex_unlock(&mtx);
        #endif
    }
    bool Mutex::TryLock()
    {
        #if defined(_WIN32)
        return WaitForSingleObject(mtx, 100) == WAIT_OBJECT_0;
        #elif defined(GEKKO)
        return LWP_MutexTryLock(mtx) == 0;
        #else
        return pthread_mutex_trylock(&mtx) == 0;
        #endif
    }
    Mutex::~Mutex()
    {
        #if defined(_WIN32)
        CloseHandle(mtx);
        #elif defined(GEKKO)
        LWP_MutexDestroy(mtx);
        #else
        pthread_mutex_destroy(&mtx);
        pthread_mutexattr_destroy(&attr);
        #endif

    }
};
