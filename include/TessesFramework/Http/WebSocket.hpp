#pragma once
#include <functional>
#include <cstdint>
#include <string>
#include <vector>
#include <cstddef>
namespace Tesses::Framework::Http
{
    class WebSocketMessage {
        public:
            std::vector<uint8_t> data;
            bool isBinary;
            WebSocketMessage();
            WebSocketMessage(std::vector<uint8_t> data);
            WebSocketMessage(const void* data, size_t len);
            WebSocketMessage(std::string message);
            std::string ToString();

             
        
    };
    void SendWebSocketMessage(std::function<void(WebSocketMessage&)> cb, std::string text);
    class WebSocketConnection {
        
        public:
            virtual void OnOpen(std::function<void(WebSocketMessage&)> sendMessage, std::function<void()> ping,std::function<void()> close)=0;
            virtual void OnReceive(WebSocketMessage& message)=0;
            virtual void OnClose(bool clean)=0;
            virtual ~WebSocketConnection();
    };
    class CallbackWebSocketConnection : public WebSocketConnection {
        public:
            std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>,std::function<void()>)> onOpen;
            std::function<void(WebSocketMessage&)> onReceive;
            std::function<void(bool)> onClose;
            CallbackWebSocketConnection();
            CallbackWebSocketConnection(std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>,std::function<void()>)> onOpen, std::function<void(WebSocketMessage&)> onReceive, std::function<void(bool)> onClose);

            void OnOpen(std::function<void(WebSocketMessage&)> sendMessage, std::function<void()> ping,std::function<void()> closeFn);
            void OnReceive(WebSocketMessage& message);
            void OnClose(bool clean);
    };
}