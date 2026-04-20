#pragma once

#include <chrono>
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <ratio>
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
    
       
    std::optional<std::string> TF_GetCommandName();
    class TF_Timer_Handle;
    //Used for internal purposes, don't use unless you want to run timer events on another thread
   
    class TF_Timer_Handler {
        private:
            std::vector<std::weak_ptr<TF_Timer_Handle>> handles;
        public:
            void Update();

            
            static std::shared_ptr<TF_Timer_Handle> Make(std::shared_ptr<TF_Timer_Handler> handler);
        friend class TF_Timer_Handle;
    };

    class TF_Timer_Handle {
        
        private:
            std::shared_ptr<TF_Timer_Handler> timerHandler;
            std::function<void()> cb;
            std::chrono::milliseconds interval;
            std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds> last;
            bool enabled;
            TF_Timer_Handle() = delete;

            TF_Timer_Handle(std::shared_ptr<TF_Timer_Handler> timerHandler);

        public:
        
            void SetCallback(std::function<void()> cb);

            int64_t GetIntervalMilliseconds();
            std::chrono::duration<int64_t,std::milli> GetIntervalDuration();

            void SetIntervalFromMilliseconds(int64_t ms);
            
            void SetIntervalFromDuration(std::chrono::milliseconds dur);
            

            void SetEnabled(bool enabled);
            bool GetEnabled();
            friend class TF_Timer_Handler;
    };


    std::shared_ptr<TF_Timer_Handle> TF_Timer();
    std::shared_ptr<TF_Timer_Handle> TF_Timer(std::function<void()> cb, int64_t interval=1000, bool enabled=true);
    std::shared_ptr<TF_Timer_Handle> TF_Timer(std::function<void()> cb, std::chrono::milliseconds interval, bool enabled=true);

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