#include "TessesFramework/Threading/Mutex.hpp"
#include <cstring>
#include <iostream>
#if defined(_WIN32)
#include <windows.h>
#elif defined(GEKKO)
#include <ogc/mutex.h>
#else
#include <pthread.h>
#endif
namespace Tesses::Framework::Threading
{
    

    class MutexHiddenFieldData : public HiddenFieldData
    {
        public:
        #if defined(_WIN32)
        HANDLE mtx;
        #elif defined(GEKKO)
        mutex_t mtx;
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
            #else
            pthread_mutex_destroy(&mtx);
            pthread_mutexattr_destroy(&attr);
            #endif
        }
    };

    Mutex::Mutex()
    {
        auto md=this->data.AllocField<MutexHiddenFieldData*>();
        #if defined(_WIN32)
        md->mtx = CreateMutex(NULL,false,NULL);
        #elif defined(GEKKO)
        md->mtx = LWP_MUTEX_NULL;
        LWP_MutexInit(&md->mtx, true);
        #else
        pthread_mutexattr_init(&md->attr);
        pthread_mutexattr_settype(&md->attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&md->mtx,&md->attr);
    
        #endif
    }
    void Mutex::Lock()
    {
        auto md = this->data.GetField<MutexHiddenFieldData*>();
        #if defined(_WIN32)
        WaitForSingleObject(md->mtx, INFINITE);
        #elif defined(GEKKO)
        LWP_MutexLock(md->mtx);
        #else
        pthread_mutex_lock(&md->mtx);
        #endif
    }
    void Mutex::Unlock()
    {
        auto md = this->data.GetField<MutexHiddenFieldData*>();
        #if defined(_WIN32)
        ReleaseMutex(md->mtx);
        #elif defined(GEKKO)
        LWP_MutexUnlock(md->mtx);
        #else
        pthread_mutex_unlock(&md->mtx);
        #endif
    }
    bool Mutex::TryLock()
    {
        auto md = this->data.GetField<MutexHiddenFieldData*>();
        #if defined(_WIN32)
        return WaitForSingleObject(md->mtx, 100) == WAIT_OBJECT_0;
        #elif defined(GEKKO)
        return LWP_MutexTryLock(md->mtx) == 0;
        #else
        return pthread_mutex_trylock(&md->mtx) == 0;
        #endif
    }
    Mutex::~Mutex()
    {
       

    }
};
