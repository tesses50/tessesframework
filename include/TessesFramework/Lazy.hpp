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
#include "Common.hpp"
#include "Threading/Mutex.hpp"
#include <functional>
namespace Tesses::Framework {
template <typename T> class Lazy {
    Threading::Mutex mtx;
    T value;
    bool hasInit = false;
    std::function<T()> init;
    std::function<void(T)> free;

  public:
    Lazy(std::function<T()> init, std::function<void(T)> free) {
        this->init = init;
        this->free = free;
    }
    Lazy(std::function<T()> init) : Lazy(init, [](T item) -> void {}) {}
    bool HasInit() {
        mtx.Lock();
        bool hI = this->hasInit;
        mtx.Unlock();
        return hI;
    }
    T &GetValue() {
        mtx.Lock();
        if (hasInit) {
            mtx.Unlock();
            return this->value;
        } else {
            this->value = this->init();
            this->hasInit = true;
            mtx.Unlock();
            return this->value;
        }
        mtx.Unlock();
    }
    ~Lazy() {
        if (hasInit)
            this->free(this->value);
    }
};
} // namespace Tesses::Framework
