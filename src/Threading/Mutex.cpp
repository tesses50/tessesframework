/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
    https://git.tesses.org/tesses50/crosslang

    Copyright (C) 2026 Mike Nolan
    SPDX-License-Identifier: GPL-3.0-or-later WITH TessesFramework-Exception-1.0

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "TessesFramework/Threading/Mutex.hpp"
#include <cstring>
#include <iostream>
#if defined(_WIN32)
#include <windows.h>
#undef min
#else
#include <pthread.h>
#endif
namespace Tesses::Framework::Threading {

#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
class MutexHiddenFieldData : public HiddenFieldData {
  public:
#if defined(_WIN32)
    CRITICAL_SECTION mtx;
#else
    pthread_mutex_t mtx;
    pthread_mutexattr_t attr;
#endif
    ~MutexHiddenFieldData() {
#if defined(_WIN32)
        DeleteCriticalSection(&mtx);
#else
        pthread_mutex_destroy(&mtx);
        pthread_mutexattr_destroy(&attr);
#endif
    }
};

class MutexCondHiddenFieldData : public HiddenFieldData {
  public:
#if defined(_WIN32)
    CONDITION_VARIABLE cond;
#else

    pthread_cond_t cond;
#endif

    ~MutexCondHiddenFieldData() {
#if defined(_WIN32)

#else
        pthread_cond_destroy(&cond);
#endif
    }
};
#endif

Mutex::Mutex() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto md = this->data.AllocField<MutexHiddenFieldData>();
#if defined(_WIN32)
    InitializeCriticalSection(&(md->mtx));
#else
    pthread_mutexattr_init(&md->attr);
    pthread_mutexattr_settype(&md->attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&md->mtx, &md->attr);

#endif
#endif
}
void Mutex::Lock() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto md = this->data.GetField<MutexHiddenFieldData *>();
#if defined(_WIN32)
    EnterCriticalSection(&(md->mtx));

#else
    pthread_mutex_lock(&md->mtx);
#endif
#endif
}
void Mutex::Unlock() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto md = this->data.GetField<MutexHiddenFieldData *>();
#if defined(_WIN32)
    LeaveCriticalSection(&(md->mtx));
#else
    pthread_mutex_unlock(&md->mtx);
#endif
#endif
}
bool Mutex::TryLock() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto md = this->data.GetField<MutexHiddenFieldData *>();
#if defined(_WIN32)
    return TryEnterCriticalSection(&md->mtx) != FALSE;
#else
    return pthread_mutex_trylock(&md->mtx) == 0;
#endif
#endif
}
Mutex::~Mutex() {}

Cond::Cond() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto md = this->data.AllocField<MutexCondHiddenFieldData>();
#if defined(_WIN32)
    InitializeConditionVariable(&md->cond);
#else
    pthread_cond_init(&md->cond, NULL);

#endif
#endif
}

void Cond::Wait(Mutex *mtx) {
    if (mtx == nullptr)
        return;
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto mcd = this->data.GetField<MutexCondHiddenFieldData *>();
    auto md = mtx->data.GetField<MutexHiddenFieldData *>();
#if defined(_WIN32)
    SleepConditionVariableCS(&mcd->cond, &md->mtx, INFINITE);
#else
    pthread_cond_wait(&mcd->cond, &md->mtx);
#endif
#endif
}
bool Cond::Wait(Mutex *mtx, uint32_t milliseconds) {
    if (mtx == nullptr)
        return false;
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto mcd = this->data.GetField<MutexCondHiddenFieldData *>();
    auto md = mtx->data.GetField<MutexHiddenFieldData *>();
#if defined(_WIN32)
    return SleepConditionVariableCS(&mcd->cond, &md->mtx, milliseconds) !=
           FALSE;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += milliseconds / 1000;
    ts.tv_nsec += (milliseconds % 1000) * 1000000L;
    if (ts.tv_nsec >= 1000000000L) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000L;
    }
    return pthread_cond_timedwait(&mcd->cond, &md->mtx, &ts) == 0;
#endif
#endif
    return false;
}
bool Cond::Wait(Mutex *mtx, Date::TimeSpan ts) {
    return Wait(mtx, (uint32_t)ts.TotalSeconds() * 1000);
}
void Cond::Signal() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto mcd = this->data.GetField<MutexCondHiddenFieldData *>();

#if defined(_WIN32)
    WakeConditionVariable(&mcd->cond);
#else
    pthread_cond_signal(&mcd->cond);
#endif
#endif
}
void Cond::Broadcast() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto mcd = this->data.GetField<MutexCondHiddenFieldData *>();

#if defined(_WIN32)
    WakeConditionVariable(&mcd->cond);
#else
    pthread_cond_broadcast(&mcd->cond);
#endif
#endif
}

Cond::~Cond() {}

}; // namespace Tesses::Framework::Threading
