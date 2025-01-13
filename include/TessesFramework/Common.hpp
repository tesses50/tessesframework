#pragma once

#include <cstring>
#include <cstdint>
#include <cstddef>
#include <string>
#include <filesystem>
#include <map>
#include <vector>


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
    void TF_Init();
    void TF_ConnectToSelf(uint16_t port);
    void TF_RunEventLoop();
    void TF_RunEventLoopItteration();
    bool TF_IsRunning();
    void TF_Quit();
}