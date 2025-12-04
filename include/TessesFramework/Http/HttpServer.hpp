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
        std::shared_ptr<Tesses::Framework::Streams::Stream> strm;
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
            uint16_t serverPort;
            std::string version;
            bool encrypted;
            ServerContext(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
            ~ServerContext();
            std::shared_ptr<Tesses::Framework::Streams::Stream> GetStream();
            std::string GetOriginalPathWithQuery();
            std::string GetUrlWithQuery();
            bool Sent();
            bool NeedToParseFormData();
            void ParseFormData(std::function<std::shared_ptr<Tesses::Framework::Streams::Stream>(std::string mime, std::string filename, std::string name)> cb);
            void ReadStream(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
            std::string ReadString();
            void SendBytes(std::vector<uint8_t> buffer);
            void SendText(std::string text);
            void SendStream(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
            void SendErrorPage(bool showPath);
            void SendNotFound();
            void SendBadRequest();
            void SendException(std::exception& ex);
            std::shared_ptr<Tesses::Framework::Streams::Stream> OpenResponseStream();
            std::shared_ptr<Tesses::Framework::Streams::Stream> OpenRequestStream();
            ServerContext& WithLastModified(Date::DateTime time);
            ServerContext& WithHeader(std::string key, std::string value);
            ServerContext& WithSingleHeader(std::string key, std::string value);
            ServerContext& WithMimeType(std::string mime);
            ServerContext& WithContentDisposition(std::string filename, bool isInline);
            ServerContext& WriteHeaders();
            ServerContext& WithLocationHeader(std::string url);
            ServerContext& WithLocationHeader(std::string url,StatusCode sc);
            void StartWebSocketSession(std::function<void(std::function<void(WebSocketMessage&)>,std::function<void()>,std::function<void()>)> onOpen, std::function<void(WebSocketMessage&)> onReceive, std::function<void(bool)> onClose);
            void StartWebSocketSession(std::shared_ptr<WebSocketConnection> connection);
            std::string GetServerRoot();
            std::string MakeAbsolute(std::string path);
            void SendRedirect(std::string url);
            void SendRedirect(std::string url, StatusCode sc);

            
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
        std::shared_ptr<Tesses::Framework::Streams::TcpServer> server;
        std::shared_ptr<IHttpServer> http;
        Tesses::Framework::Threading::Thread* thrd;

        bool showIPs;
        bool showARTL;
    
        public:
            HttpServer(std::shared_ptr<Tesses::Framework::Streams::TcpServer> tcpServer, std::shared_ptr<IHttpServer> http, bool showIPs=true);
            HttpServer(uint16_t port, std::shared_ptr<IHttpServer> http, bool showIPs=true);
            HttpServer(std::string unixPath, std::shared_ptr<IHttpServer> http);
            uint16_t GetPort();
            void StartAccepting();
            static void Process(std::shared_ptr<Tesses::Framework::Streams::Stream> strm, std::shared_ptr<IHttpServer> server, std::string ip, uint16_t port,uint16_t serverPort, bool encrypted);
            ~HttpServer();
    };
}   