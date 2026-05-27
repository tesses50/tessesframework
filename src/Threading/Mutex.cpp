/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
   https://git.tesses.org/tesses50/crosslang Copyright (C) 2026 Mike Nolan

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
    HANDLE mtx;
#else
    pthread_mutex_t mtx;
    pthread_mutexattr_t attr;
#endif
    ~MutexHiddenFieldData() {
#if defined(_WIN32)
        CloseHandle(mtx);
#else
        pthread_mutex_destroy(&mtx);
        pthread_mutexattr_destroy(&attr);
#endif
    }
};
#endif
Mutex::Mutex() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto md = this->data.AllocField<MutexHiddenFieldData>();
#if defined(_WIN32)
    md->mtx = CreateMutex(NULL, false, NULL);
#elif defined(GEKKO)
    md->mtx = LWP_MUTEX_NULL;
    LWP_MutexInit(&md->mtx, true);

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
    WaitForSingleObject(md->mtx, INFINITE);

#else
    pthread_mutex_lock(&md->mtx);
#endif
#endif
}
void Mutex::Unlock() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto md = this->data.GetField<MutexHiddenFieldData *>();
#if defined(_WIN32)
    ReleaseMutex(md->mtx);
#else
    pthread_mutex_unlock(&md->mtx);
#endif
#endif
}
bool Mutex::TryLock() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto md = this->data.GetField<MutexHiddenFieldData *>();
#if defined(_WIN32)
    return WaitForSingleObject(md->mtx, 100) == WAIT_OBJECT_0;

#else
    return pthread_mutex_trylock(&md->mtx) == 0;
#endif
#endif
}
Mutex::~Mutex() {}
}; // namespace Tesses::Framework::Threading
