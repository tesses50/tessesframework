#pragma once
#include "Common.hpp"
#include "Threading/Mutex.hpp"
#include <functional>
namespace Tesses::Framework {
template<typename T>
    class Lazy {
        Threading::Mutex mtx;
        T value;
        bool hasInit=false;
        std::function<T()> init;
        std::function<void(T)> free;
        public:
            Lazy(std::function<T()> init, std::function<void(T)> free)
            {
                this->init = init;
                this->free = free;
            }
            Lazy(std::function<T()> init) : Lazy(init, [](T item)->void {})
            {

            }
            bool HasInit()
            {
                mtx.Lock();
                bool hI = this->hasInit;
                mtx.Unlock();
                return hI;
            }
            T& GetValue()
            {   
                mtx.Lock();
                if(hasInit)
                {
                    mtx.Unlock();
                    return this->value;    
                }
                else
                {
                    this->value = this->init();
                    mtx.Unlock();
                    return this->value;
                }
                mtx.Unlock();
            }
            ~Lazy()
            {
                if(hasInit) this->free(this->value);
            }
    };
}