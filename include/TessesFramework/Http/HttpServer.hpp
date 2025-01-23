#pragma once
#include "../Streams/NetworkStream.hpp"

#include "HttpUtils.hpp"
#include "../Threading/Thread.hpp"
#include <unordered_map>
namespace Tesses::Framework::Http
{
    class ServerContextData {
        public:
            virtual ~ServerContextData();
    };

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

    class WebSocketConnection {
        public:
            virtual void OnOpen(std::function<void(WebSocketMessage&)> sendMessage, std::function<void()> ping)=0;
            virtual void OnReceive(WebSocketMessage& message)=0;
            virtual void OnClose(bool clean)=0;
            virtual ~WebSocketConnection();
    };
    class CallbackWebSocketConnection : public WebSocketConnection {
        public:
            std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>)> onOpen;
            std::function<void(WebSocketMessage&)> onReceive;
            std::function<void(bool)> onClose;
            CallbackWebSocketConnection();
            CallbackWebSocketConnection(std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>)> onOpen, std::function<void(WebSocketMessage&)> onReceive, std::function<void(bool)> onClose);

            void OnOpen(std::function<void(WebSocketMessage&)> sendMessage, std::function<void()> ping);
            void OnReceive(WebSocketMessage& message);
            void OnClose(bool clean);
    };

    class ServerContext {
        bool sent;
        Tesses::Framework::Streams::Stream* strm;
        std::map<std::string,ServerContextData*> data;
        public:
            HttpDictionary requestHeaders;
            HttpDictionary responseHeaders;
            HttpDictionary queryParams;
            std::string path;
            std::string originalPath;
            std::string method;
            StatusCode statusCode;
            std::string ip;
            uint16_t port;
            std::string version;
            bool encrypted;
            ServerContext(Tesses::Framework::Streams::Stream* strm);
            ~ServerContext();
            Tesses::Framework::Streams::Stream& GetStream();
            std::string GetOriginalPathWithQuery();
            std::string GetUrlWithQuery();
            bool Sent();
            bool NeedToParseFormData();
            void ParseFormData(std::function<Tesses::Framework::Streams::Stream*(std::string mime, std::string filename, std::string name)> cb);
            void ReadStream(Tesses::Framework::Streams::Stream& strm);
            void ReadStream(Tesses::Framework::Streams::Stream* strm);
            std::string ReadString();
            void SendBytes(std::vector<uint8_t> buffer);
            void SendText(std::string text);
            void SendStream(Tesses::Framework::Streams::Stream& strm);
            void SendStream(Tesses::Framework::Streams::Stream* strm);
            void SendErrorPage(bool showPath);
            void SendNotFound();
            void SendBadRequest();
            void SendException(std::exception& ex);
            Tesses::Framework::Streams::Stream* OpenResponseStream();
            Tesses::Framework::Streams::Stream* OpenRequestStream();
            ServerContext& WithHeader(std::string key, std::string value);
            ServerContext& WithSingleHeader(std::string key, std::string value);
            ServerContext& WithMimeType(std::string mime);
            ServerContext& WithContentDisposition(std::string filename, bool isInline);
            ServerContext& WriteHeaders();
            void StartWebSocketSession(std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>)> onOpen, std::function<void(WebSocketMessage&)> onReceive, std::function<void(bool)> onClose);
            void StartWebSocketSession(WebSocketConnection& connection);
            
            template<class T>
            T* GetServerContentData(std::string tag)
            {
                std::string name = typeid(T).name();
                name.push_back(' ');
                name.append(tag);
                if(data.count(name) > 0) return dynamic_cast<T*>(data[name]);
                T* item = new T();
                data[name] = item;
                return item;
            }
    };

    class IHttpServer {
        public:
            virtual bool Handle(ServerContext& ctx)=0;
            virtual ~IHttpServer();
    };

    class HttpServer {
        Tesses::Framework::Streams::TcpServer* server;
        IHttpServer* http;
        Tesses::Framework::Threading::Thread* thrd;
        bool owns;
        uint16_t port;
        public:
            HttpServer(uint16_t port, IHttpServer& http);
            HttpServer(uint16_t port, IHttpServer* http, bool owns);
            void StartAccepting();
            static void Process(Tesses::Framework::Streams::Stream& strm, IHttpServer& server, std::string ip, uint16_t port, bool encrypted);
            ~HttpServer();
    };
}   