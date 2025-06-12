#pragma once

#include <cstring>
#include <cstdint>
#include <cstddef>
#include <string>
#include <filesystem>
#include <map>
#include <vector>
#include <functional>
#include "Threading/Mutex.hpp"
    class TextException : public std::exception {
        
        std::string error_message;
        public:
           
            TextException(std::string ex)
            {
                error_message = "TextException: ";
                error_message.append(ex);
            }

           
            
        const char * what() const noexcept override
        {
            return error_message.c_str();
        }
    };
namespace Tesses::Framework
{
        template<typename...TArgs>
    class Event {
        public:
            virtual void Invoke(TArgs... args)=0;
            virtual ~Event()
            {}
    };
    template<typename...TArgs>
    class FunctionalEvent : public Event<TArgs...> {
        std::function<void(TArgs...)> cb;
        public:
            FunctionalEvent(std::function<void(TArgs...)> cb)
            {
                this->cb = cb;
            }
            void Invoke(TArgs... args)
            {
                this->cb(args...);
            }
    };

    template<typename...TArgs>
    class EventList : public Event<TArgs...> {
        Threading::Mutex mtx;
        std::vector<std::shared_ptr<Event<TArgs...>>> items;
        public:
            void operator+=(std::shared_ptr<Event<TArgs...>> event)
            {
                mtx.Lock();
                for(std::shared_ptr<Event<TArgs...>>& item : this->items)
                {
                    if(item.get() == event.get()) return;
                }
                this->items.push_back(event);
                mtx.Unlock();
            }
            void operator-=(std::shared_ptr<Event<TArgs...>> event)
            {
                mtx.Lock();
                for(auto i = this->items.begin(); i != this->items.end(); i++)
                {
                    if(i->get() == event.get())
                    {
                        this->items.erase(i);
                        return;
                    }
                }
                mtx.Lock();
            }
            void Invoke(TArgs... args)
            {
                mtx.Lock();
                for(auto& item : this->items)
                {
                    item->Invoke(args...);
                }
                mtx.Unlock();
            }
            void Remove(std::function<bool(std::shared_ptr<Event<TArgs...>>)> cb)
            {
                for(auto index = this->items.begin(); index != this->items.end(); index++)
                {
                    if(cb(*index)) 
                    {
                        this->items.erase(index);
                        index--;
                    }
                }
            }
    };

    extern EventList<uint64_t> OnItteraton;
    void TF_Init();
    void TF_InitWithConsole();
    void TF_ConnectToSelf(uint16_t port);
    void TF_RunEventLoop();
    void TF_RunEventLoopItteration();
    bool TF_IsRunning();
    void TF_SetIsRunning(bool _isRunning);
    void TF_Quit();
    bool TF_GetConsoleEventsEnabled();
    void TF_SetConsoleEventsEnabled(bool flag);
    void TF_InitConsole();
   
    #if defined(TESSESFRAMEWORK_LOGTOFILE)
    void TF_Log(std::string dataToLog);
    #endif


    #if defined(TESSESFRAMEWORK_LOGTOFILE)
    #define TF_LOG(log) Tesses::Framework::TF_Log(log)
    #else
    #define TF_LOG(log)
    #endif
}