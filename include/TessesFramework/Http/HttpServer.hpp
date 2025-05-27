#pragma once
#include "../Streams/NetworkStream.hpp"

#include "HttpUtils.hpp"
#include "../Threading/Thread.hpp"
#include "../Date/Date.hpp"
#include <unordered_map>
#include "WebSocket.hpp"
namespace Tesses::Framework::Http
{
    class ServerContextData {
        public:
            virtual ~ServerContextData();
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
            ServerContext& WithLastModified(Date::DateTime time);
            ServerContext& WithHeader(std::string key, std::string value);
            ServerContext& WithSingleHeader(std::string key, std::string value);
            ServerContext& WithMimeType(std::string mime);
            ServerContext& WithContentDisposition(std::string filename, bool isInline);
            ServerContext& WriteHeaders();
            void StartWebSocketSession(std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>,std::function<void()>)> onOpen, std::function<void(WebSocketMessage&)> onReceive, std::function<void(bool)> onClose);
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

        bool ownsTCP;
        bool ownsHttp;
        bool showIPs;
        bool showARTL;
    
        public:
            HttpServer(Tesses::Framework::Streams::TcpServer& tcpServer, IHttpServer& http, bool showIPs=true);
            HttpServer(Tesses::Framework::Streams::TcpServer* tcpServer, bool ownsTCP, IHttpServer& http, bool showIPs=true);
            HttpServer(Tesses::Framework::Streams::TcpServer& tcpServer, IHttpServer* http, bool ownsHttpServer, bool showIPs=true);
            HttpServer(Tesses::Framework::Streams::TcpServer* tcpServer, bool ownsTCP, IHttpServer* http, bool ownsHttpServer, bool showIPs=true);
            HttpServer(uint16_t port, IHttpServer& http, bool showIPs=true);
            HttpServer(uint16_t port, IHttpServer* http, bool owns, bool showIPs=true);
            HttpServer(std::string unixPath, IHttpServer& http);
            HttpServer(std::string unixPath, IHttpServer* http, bool owns);
            uint16_t GetPort();
            void StartAccepting();
            static void Process(Tesses::Framework::Streams::Stream& strm, IHttpServer& server, std::string ip, uint16_t port, bool encrypted);
            ~HttpServer();
    };
}   