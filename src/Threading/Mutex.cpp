#include "TessesFramework/Threading/Mutex.hpp"
#include <cstring>
namespace Tesses::Framework::Threading
{
    Mutex::Mutex()
    {
        #if defined(GEKKO)
        mtx = LWP_MUTEX_NULL;
        LWP_MutexInit(&mtx, true);
        #else
        memset(&mtx, 0, sizeof(mtx_t));
        mtx_init(&mtx, mtx_recursive);

        #endif
    }
    void Mutex::Lock()
    {
        #if defined(GEKKO)
        LWP_MutexLock(mtx);
        #else
        mtx_lock(&mtx);
        #endif
    }
    void Mutex::Unlock()
    {
        #if defined(GEKKO)
        LWP_MutexUnlock(mtx);
        #else
        mtx_unlock(&mtx);
        #endif
    }
    bool Mutex::TryLock()
    {
        #if defined(GEKKO)
        return LWP_MutexTryLock(mtx) == 0;
        #else
        return mtx_trylock(&mtx) == thrd_success;
        #endif
    }
    Mutex::~Mutex()
    {
        #if defined(GEKKO)
        LWP_MutexDestroy(mtx);
        #else
        mtx_destroy(&mtx);
        #endif

    }
};