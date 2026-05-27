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

#include "TessesFramework/Threading/Thread.hpp"
#include "TessesFramework/Common.hpp"
#include "TessesFramework/Threading/Mutex.hpp"
#include <iostream>
#include <memory>
#if defined(__SWITCH__)
extern "C" {
#include <pthread.h>
#include <switch.h>
}
#endif
namespace Tesses::Framework::Threading {

#if defined(TESSESFRAMEWORK_ENABLE_THREADING)

#if defined(__SWITCH__)
Mutex needed_to_be_joined_mtx;
class NeedToBeJoinnedThread {

    static void *cb(void *data) {

        auto ntbjt = static_cast<NeedToBeJoinnedThread *>(data);

        ntbjt->hasInvoked = true;
        TF_LOG("About to call thread func");
        if (ntbjt->_cb)
            ntbjt->_cb();
        TF_LOG("Finished calling thread func");
        ntbjt->hasExited = true;

        return NULL;
    }
    std::function<void()> _cb;
    std::atomic<bool> hasInvoked = false;

    pthread_t thrd;

  public:
    NeedToBeJoinnedThread(std::function<void()> cb) {
        this->_cb = cb;
        joinned = false;
        joinning = false;
        hasExited = false;

        pthread_create(&thrd, NULL, this->cb, static_cast<void *>(this));

    }
    std::atomic<bool> joinned;
    std::atomic<bool> joinning;
    std::atomic<bool> hasExited;
    void Join();
    void WaitTillInvoked() {
        while (!hasInvoked)
            ;
        TF_LOG("Invoked!");
    }
};

void NeedToBeJoinnedThread::Join() {
    if (joinning) {
        return;
    }
    joinning = true;
    pthread_join(this->thrd, NULL);

    joinned = true;
    // start the joinning process
}

std::vector<std::shared_ptr<NeedToBeJoinnedThread>> needToBeJoinnedThread;
void JoinAllThreads() {
    needed_to_be_joined_mtx.Lock();
    for (auto item : needToBeJoinnedThread) {
        item->Join();
    }
    needToBeJoinnedThread.clear();
    needed_to_be_joined_mtx.Unlock();
}
void LookForFinishedThreads() {

    needed_to_be_joined_mtx.Lock();
    for (auto index = needToBeJoinnedThread.begin();
         index < needToBeJoinnedThread.end(); index++) {
        auto &idx = *index;
        if (idx->hasExited) {
            if (idx->joinning)
                while (!idx->joinned)
                    ;
            TF_LOG("ABOUT TO JOIN");
            idx->Join();
            TF_LOG("JOINNED");
            needToBeJoinnedThread.erase(index);
            index--;
        }
    }
    needed_to_be_joined_mtx.Unlock();
}

#endif
class ThreadHiddenFieldData : public HiddenFieldData {
  public:
#if defined(_WIN32)

    HANDLE thrd;
    DWORD thrdId;

#elif defined(__SWITCH__)
    std::shared_ptr<NeedToBeJoinnedThread> thread;
#else
    pthread_t thrd;
#endif

    std::function<void()> fn;

    std::atomic<bool> hasInvoked;
};

#if defined(_WIN32)
static DWORD __stdcall cb(LPVOID data)
#elif defined(__SWITCH__)
static void cb(void *data)
#else
static void *cb(void *data)
#endif
{
    auto thrd = static_cast<ThreadHiddenFieldData *>(data);

    auto fn = thrd->fn;
    thrd->hasInvoked = true;
    fn();
#if !defined(_WIN32) && !defined(__SWITCH__)
    return NULL;
#elif (__SWITCH__)

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
#elif defined(__SWITCH__)
    data->thread = std::make_shared<NeedToBeJoinnedThread>(fn);
    data->thread->WaitTillInvoked();
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
#elif defined(__SWITCH__)
    TF_LOG("Detaching");
    needed_to_be_joined_mtx.Lock();
    needToBeJoinnedThread.push_back(data->thread);
    needed_to_be_joined_mtx.Unlock();

    TF_LOG("Detached!");
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
#elif defined(__SWITCH__)
    data->thread->Join();
#else
    pthread_join(data->thrd, NULL);
#endif
#endif
}
} // namespace Tesses::Framework::Threading
