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

#pragma once
#include "Mutex.hpp"
#include "Thread.hpp"
#include <functional>
#include <queue>
#include <vector>

namespace Tesses::Framework::Threading {

class ThreadPool {
    std::vector<Thread *> threads;
    std::queue<std::function<void(size_t)>> callbacks;
    Mutex mtx;
    volatile bool isRunning;

  public:
    static size_t GetNumberOfCores();
    ThreadPool(size_t threads);
    size_t ThreadCount();
    bool Empty();
    void Schedule(std::function<void(size_t)> cb);
    ~ThreadPool();
};
} // namespace Tesses::Framework::Threading