#include "TessesFramework/Threading/Mutex.hpp"
#include <cstring>
#include <iostream>
#if defined(_WIN32)
#include <windows.h>
#undef min
#elif defined(GEKKO)
#include <ogc/mutex.h>
#elif defined(__SWITCH__)
extern "C" {
#include <switch/kernel/mutex.h>
}
#else
#include <pthread.h>
#endif
namespace Tesses::Framework::Threading
{
    
    #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    class MutexHiddenFieldData : public HiddenFieldData
    {
        public:
        #if defined(_WIN32)
        HANDLE mtx;
        #elif defined(GEKKO)
        mutex_t mtx;
        #elif defined(__SWITCH__)
        RMutex mtx;
        #else
        pthread_mutex_t mtx;
        pthread_mutexattr_t attr;
        #endif
        ~MutexHiddenFieldData()
        {
            #if defined(_WIN32)
            CloseHandle(mtx);
            #elif defined(GEKKO)
            LWP_MutexDestroy(mtx);
            #elif defined(__SWITCH__)
            
            #else
            pthread_mutex_destroy(&mtx);
            pthread_mutexattr_destroy(&attr);
            #endif
        }
    };
    #endif
    Mutex::Mutex()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        auto md=this->data.AllocField<MutexHiddenFieldData>();
        #if defined(_WIN32)
        md->mtx = CreateMutex(NULL,false,NULL);
        #elif defined(GEKKO)
        md->mtx = LWP_MUTEX_NULL;
        LWP_MutexInit(&md->mtx, true);
        #elif defined(__SWITCH__)
        rmutexInit(&md->mtx);
        #else
        pthread_mutexattr_init(&md->attr);
        pthread_mutexattr_settype(&md->attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&md->mtx,&md->attr);
    
        #endif
        #endif
    }
    void Mutex::Lock()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        auto md = this->data.GetField<MutexHiddenFieldData*>();
        #if defined(_WIN32)
        WaitForSingleObject(md->mtx, INFINITE);
        #elif defined(GEKKO)
        LWP_MutexLock(md->mtx);
        #elif defined(__SWITCH__)
        rmutexLock(&md->mtx);
        #else
        pthread_mutex_lock(&md->mtx);
        #endif
        #endif
    }
    void Mutex::Unlock()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        auto md = this->data.GetField<MutexHiddenFieldData*>();
        #if defined(_WIN32)
        ReleaseMutex(md->mtx);
        #elif defined(GEKKO)
        LWP_MutexUnlock(md->mtx);
        #elif defined(__SWITCH__)
        rmutexUnlock(&md->mtx);
        #else
        pthread_mutex_unlock(&md->mtx);
        #endif
        #endif
    }
    bool Mutex::TryLock()
    {
        #if defined(TESSESFRAMEWORK_ENABLE_THREADING)
        auto md = this->data.GetField<MutexHiddenFieldData*>();
        #if defined(_WIN32)
        return WaitForSingleObject(md->mtx, 100) == WAIT_OBJECT_0;
        #elif defined(GEKKO)
        return LWP_MutexTryLock(md->mtx) == 0;
        #elif defined(__SWITCH__)
        return rmutexTryLock(&md->mtx);
        #else
        return pthread_mutex_trylock(&md->mtx) == 0;
        #endif
        #endif
    }
    Mutex::~Mutex()
    {
       

    }
};
