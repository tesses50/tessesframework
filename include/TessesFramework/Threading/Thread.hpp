#pragma once
#include <functional>
#if defined(_WIN32)
#include <windows.h>
#undef min
#elif defined(GEKKO)
#include <ogc/lwp.h>
#elif defined(__SWITCH__)
#else
#include <pthread.h>
#endif
#include <atomic>
#include "../HiddenField.hpp"
namespace Tesses::Framework::Threading
{
    class Thread 
    {
        HiddenField data;
        
        public:
        Thread(std::function<void()> fn);
        void Join();
        void Detach();
    };
}