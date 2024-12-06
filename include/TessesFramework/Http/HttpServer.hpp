#pragma once
#include "../Streams/NetworkStream.hpp"

#include "HttpUtils.hpp"
#include "../Threading/Thread.hpp"
namespace Tesses::Framework::Http
{
    
    class ServerContext {
        bool sent;
        Tesses::Framework::Streams::Stream* strm;
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