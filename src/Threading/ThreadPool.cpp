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

#include "TessesFramework/Threading/ThreadPool.hpp"
#if !defined(GEKKO)
#include <thread>
#endif
namespace Tesses::Framework::Threading {
size_t ThreadPool::GetNumberOfCores() {
#if defined(GEKKO)
    return 1;
#elif defined(TESSESFRAMEWORK_ENABLE_THREADING)
    return (size_t)std::thread::hardware_concurrency();
#else
    return 1;
#endif
}
size_t ThreadPool::ThreadCount() { return this->threads.size(); }
bool ThreadPool::Empty() {
    bool qie;
    this->mtx.Lock();
    qie = this->callbacks.empty();
    this->mtx.Unlock();
    return qie;
}
ThreadPool::ThreadPool(size_t threads) {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    this->isRunning = true;
    for (size_t i = 0; i < threads; i++) {
        this->threads.push_back(new Thread([this, i]() -> void {
            while (true) {
                this->mtx.Lock();

                if (!this->isRunning) {
                    this->mtx.Unlock();
                    return;
                }

                std::function<void(size_t)> fn = nullptr;

                if (!this->callbacks.empty()) {
                    fn = this->callbacks.front();
                    this->callbacks.pop();
                }
                this->mtx.Unlock();
                if (fn)
                    fn(i);
            }
        }));
    }
#endif
}
void ThreadPool::Schedule(std::function<void(size_t)> cb) {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    this->mtx.Lock();
    this->callbacks.push(cb);
    this->mtx.Unlock();
#endif
}
ThreadPool::~ThreadPool() {
#if defined(TESSESFRAMEWORK_ENABLE_THREADING)
    while (true) {
        this->mtx.Lock();
        auto emp = this->callbacks.empty();
        if (emp)
            this->isRunning = false;
        this->mtx.Unlock();
        if (emp)
            break;
    }

    for (auto item : this->threads) {
        item->Join();
        delete item;
    }
#endif
}
} // namespace Tesses::Framework::Threading