#pragma once

#include "../HiddenField.hpp"
namespace Tesses::Framework::Threading
{
    class Mutex {
        HiddenField data;
        public:
            Mutex();
            void Lock();
            void Unlock();
            bool TryLock();
            ~Mutex();
    };
}