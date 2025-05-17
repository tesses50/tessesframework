#include "TessesFramework/Http/WebSocket.hpp"
namespace Tesses::Framework::Http
{

    CallbackWebSocketConnection::CallbackWebSocketConnection()
    {

    }
    CallbackWebSocketConnection::CallbackWebSocketConnection(std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>,std::function<void()>)> onOpen, std::function<void(WebSocketMessage&)> onReceive, std::function<void(bool)> onClose)
    {
        this->onOpen = onOpen;
        this->onReceive = onReceive;
        this->onClose = onClose;
    }

    void CallbackWebSocketConnection::OnOpen(std::function<void(WebSocketMessage&)> sendMessage, std::function<void()> ping, std::function<void()> closeConnection)
    {
        if(this->onOpen)
            this->onOpen(sendMessage,ping,closeConnection);
    }
    void CallbackWebSocketConnection::OnReceive(WebSocketMessage& message)
    {
        if(this->onReceive)
            this->onReceive(message);
    }
    void CallbackWebSocketConnection::OnClose(bool clean)
    {
        if(this->onClose)
            this->onClose(clean);
    }


    WebSocketMessage::WebSocketMessage()
    {
        this->isBinary=false;
        this->data={};
    }
    WebSocketMessage::WebSocketMessage(std::vector<uint8_t> data)
    {
        this->isBinary = true;
        this->data = data;
    }
    WebSocketMessage::WebSocketMessage(const void* data, size_t len)
    {
        this->isBinary=true;
        this->data={};
        this->data.insert(this->data.end(),(uint8_t*)data,((uint8_t*)data)+len);
    }
    WebSocketMessage::WebSocketMessage(std::string message)
    {
        this->isBinary=false;
        this->data={};
        this->data.insert(this->data.end(),message.begin(), message.end());
    }
    std::string WebSocketMessage::ToString()
    {
        std::string str = {};
        str.insert(str.end(),this->data.begin(),this->data.end());
        return str;
    }

    void SendWebSocketMessage(std::function<void(WebSocketMessage&)> cb, std::string text)
    {
        WebSocketMessage message(text);
        cb(message);
    }
}