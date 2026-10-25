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

#pragma once

#include "Threading/Mutex.hpp"
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ratio>
#include <string>
#include <vector>



namespace Tesses::Framework {

template <typename T>
class TF_Span {
  public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    constexpr TF_Span(pointer data, size_type size) : data_(data), size_(size) {}
    template <std::size_t N>
    constexpr TF_Span(T (&arr)[N]) : data_(arr), size_(N) {}
    template <typename Container>
    constexpr TF_Span(const Container& container) : data_(container.data()), size_(container.size()) {
      static_assert(std::is_same<typename Container::value_type, T>::value,"Container value_type must match Span's type");
    }
    reference operator[](size_type index) {
            return data_[index];
        }

        const_reference operator[](size_type index) const {
            return data_[index];
        }

        const_reference at(size_type index) const {
            if (index >= size_) {
                throw std::out_of_range("Index out of range");
            }
            return data_[index];
        }

        // Data and size
        pointer data() noexcept { return data_; }
        const_pointer data() const noexcept { return data_; }

        size_type size() const noexcept { return size_; }
        bool empty() const noexcept { return size_ == 0; }

        // Iterators
        pointer begin() noexcept { return data_; }
        pointer end() noexcept { return data_ + size_; }

        const_pointer begin() const noexcept { return data_; }
        const_pointer end() const noexcept { return data_ + size_; }
        TF_Span subspan(size_type offset, size_type count = static_cast<size_type>(-1)) const {
            if (offset > size_) {
                throw std::out_of_range("Offset out of range");
            }
            count = (count == static_cast<size_type>(-1)) ? (size_ - offset) : count;
            if (count > size_ - offset) {
                throw std::out_of_range("Count exceeds remaining elements");
            }
            return TF_Span(data_ + offset, count);
        }
      

    private:
      pointer data_;
      size_type size_;
};
class TF_Timer_Handle;
// Used for internal purposes, don't use unless you want to run timer events on
// another thread

class TF_Timer_Handler {
  private:
    std::vector<std::weak_ptr<TF_Timer_Handle>> handles;

  public:
    void Update();

    static std::shared_ptr<TF_Timer_Handle>
    Make(std::shared_ptr<TF_Timer_Handler> handler);
    friend class TF_Timer_Handle;
};

class TF_Timer_Handle {

  private:
    std::shared_ptr<TF_Timer_Handler> timerHandler;
    std::function<void()> cb;
    std::chrono::milliseconds interval;
    std::chrono::time_point<std::chrono::steady_clock,
                            std::chrono::milliseconds>
        last;
    bool enabled;
    TF_Timer_Handle() = delete;

    TF_Timer_Handle(std::shared_ptr<TF_Timer_Handler> timerHandler);

  public:
    void SetCallback(std::function<void()> cb);

    int64_t GetIntervalMilliseconds();
    std::chrono::duration<int64_t, std::milli> GetIntervalDuration();

    void SetIntervalFromMilliseconds(int64_t ms);

    void SetIntervalFromDuration(std::chrono::milliseconds dur);

    void SetEnabled(bool enabled);
    bool GetEnabled();
    friend class TF_Timer_Handler;
};

std::shared_ptr<TF_Timer_Handle> TF_Timer();
std::shared_ptr<TF_Timer_Handle> TF_Timer(std::function<void()> cb,
                                          int64_t interval = 1000,
                                          bool enabled = true);
std::shared_ptr<TF_Timer_Handle> TF_Timer(std::function<void()> cb,
                                          std::chrono::milliseconds interval,
                                          bool enabled = true);

void TF_Init();
void TF_InitWithConsole();
std::string TF_GetExecutableName();
void TF_AllowPortable();
void TF_AllowPortable(std::string argv0);
void TF_ConnectToSelf(uint16_t port);
void TF_InitEventLoop();
void TF_RunEventLoop();
void TF_RunEventLoopItteration();
bool TF_IsRunning();
void TF_Sleep(uint32_t sleepMS);
void TF_SetIsRunning(bool _isRunning);
void TF_Quit();
bool TF_GetConsoleEventsEnabled();
void TF_SetConsoleEventsEnabled(bool flag);
void TF_InitConsole();
void TF_Invoke(std::function<void()> cb);
std::string TF_FileSize(uint64_t bytes, bool usesBin = true);

#if defined(TESSESFRAMEWORK_LOGTOFILE)
void TF_Log(std::string dataToLog);
#endif

#if defined(TESSESFRAMEWORK_LOGTOFILE)
#define TF_LOG(log) Tesses::Framework::TF_Log(log)
#else
#define TF_LOG(log)
#endif
} // namespace Tesses::Framework