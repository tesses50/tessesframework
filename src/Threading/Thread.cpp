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

#include "TessesFramework/Threading/Thread.hpp"
#include "TessesFramework/Common.hpp"
#include "TessesFramework/Threading/Mutex.hpp"
#include <iostream>
#include <memory>
#if defined(_WIN32)
#include <windows.h>
#undef min
#elif defined(__SWITCH__)
extern "C" {
#include <pthread.h>
#include <switch.h>
}
#else
#include <pthread.h>
#endif

namespace Tesses::Framework::Threading {

#if defined(TESSESFRAMEWORK_ENABLE_THREADING)

class ThreadHiddenFieldData : public HiddenFieldData {
  public:
#if defined(_WIN32)

    HANDLE thrd;
    DWORD thrdId;


#else
    pthread_t thrd;
#endif

    std::function<void()> fn;

    std::atomic<bool> hasInvoked;
};

#if defined(_WIN32)
static DWORD __stdcall cb(LPVOID data)
#else
static void *cb(void *data)
#endif
{
    auto thrd = static_cast<ThreadHiddenFieldData *>(data);

    auto fn = thrd->fn;
    thrd->hasInvoked = true;
    fn();
#if !defined(_WIN32)
    return NULL;
#else
    return 0;
#endif
}
#endif

Thread::Thread(std::function<void()> fn) {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto data = this->data.AllocField<ThreadHiddenFieldData>();
    data->hasInvoked = false;
    data->fn = fn;
#if defined(_WIN32)
    data->thrd =
        CreateThread(NULL, 0, cb, static_cast<LPVOID>(data), 0, &data->thrdId);
    while (!data->hasInvoked)
        ;
    // threadCreate(,cb,static_cast<void*>(data),NULL,8000000,0x00,-2);
#else
    pthread_create(&data->thrd, NULL, cb, static_cast<void *>(data));
    while (!data->hasInvoked)
        ;
    // thrd_create(&thrd, cb, static_cast<void*>(this));
#endif

#endif
}
void Thread::Detach() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto data = this->data.GetField<ThreadHiddenFieldData *>();

#if defined(_WIN32)
    CloseHandle(data->thrd);
#else
    pthread_detach(data->thrd);
#endif

#endif
}

void Thread::Join() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    auto data = this->data.GetField<ThreadHiddenFieldData *>();
#if defined(_WIN32)
    WaitForSingleObject(data->thrd, INFINITE);
#else
    pthread_join(data->thrd, NULL);
#endif
#endif
}
} // namespace Tesses::Framework::Threading
