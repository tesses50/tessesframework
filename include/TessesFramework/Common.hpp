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
#include <optional>
#include <atomic>
   
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
                    if(item.get() == event.get()) {
                        mtx.Unlock();
                        return;
                    }
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
                        mtx.Unlock();
                        return;
                    }
                }
                mtx.Unlock();
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
    std::optional<std::string> TF_GetCommandName();

    void TF_Init();
    void TF_InitWithConsole();
    void TF_AllowPortable(std::string argv0);
    void TF_ConnectToSelf(uint16_t port);
    void TF_InitEventLoop();
    void TF_RunEventLoop();
    void TF_RunEventLoopItteration();
    bool TF_IsRunning();
    void TF_SetIsRunning(bool _isRunning);
    void TF_Quit();
    bool TF_GetConsoleEventsEnabled();
    void TF_SetConsoleEventsEnabled(bool flag);
    void TF_InitConsole();
    void TF_Invoke(std::function<void()> cb);
    std::string TF_FileSize(uint64_t bytes, bool usesBin=true);
   
    #if defined(TESSESFRAMEWORK_LOGTOFILE)
    void TF_Log(std::string dataToLog);
    #endif


    #if defined(TESSESFRAMEWORK_LOGTOFILE)
    #define TF_LOG(log) Tesses::Framework::TF_Log(log)
    #else
    #define TF_LOG(log)
    #endif
}