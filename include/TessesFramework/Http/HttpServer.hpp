/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
   https://git.tesses.org/tesses50/crosslang Copyright (C) 2026 Mike Nolan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "../Streams/NetworkStream.hpp"

#include "../Date/Date.hpp"
#include "../Threading/Thread.hpp"
#include "HttpUtils.hpp"
#include "WebSocket.hpp"
#include <queue>
#include <unordered_map>
namespace Tesses::Framework::Http {
class ServerContextData {
  public:
    virtual ~ServerContextData();
};
class ServerContext;
class ServerSentEvents {
    std::vector<std::shared_ptr<Tesses::Framework::Streams::Stream>> strms;
    Tesses::Framework::Threading::Mutex mtx;

  private:
    void SendEventRaw(const std::string &evt);

  public:
    void SendRetry(uint32_t ms);
    void SendRetry(std::chrono::milliseconds ms);
    void SendRetry(Tesses::Framework::Date::TimeSpan ts);
    void SendData(const std::string &message);
    void SendData(const std::string &message, const std::string &dataType);
    void SendId(const std::string &idVal);
    void SendCustomEvent(const std::string &type, const std::string &value);
    void SendComment(const std::string &comment);
    friend class ServerContext;
};

class ServerContext {

    bool sent;
    bool debug;
    std::vector<std::shared_ptr<ServerSentEvents>> sse;
    std::shared_ptr<Tesses::Framework::Streams::Stream> strm;
    std::map<std::string, ServerContextData *> data;
    std::queue<std::function<bool(ServerContext &ctx)>> headerhandlers;

  public:
    HttpDictionary requestHeaders;
    HttpDictionary responseHeaders;
    HttpDictionary queryParams;
    // used by path
    HttpDictionary pathArguments;
    std::string path;
    std::string originalPath;
    std::string method;
    StatusCode statusCode;
    std::string ip;
    uint16_t port;
    uint16_t serverPort;
    std::string version;
    bool encrypted;
    ServerContext(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,
                  bool debug = false);
    ~ServerContext();
    std::shared_ptr<Tesses::Framework::Streams::Stream> GetStream();
    std::string GetOriginalPathWithQuery();
    std::string GetUrlWithQuery();
    bool Sent();
    bool NeedToParseFormData();
    void ParseFormData(
        std::function<std::shared_ptr<Tesses::Framework::Streams::Stream>(
            std::string mime, std::string filename, std::string name)>
            cb);
    void ReadStream(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    std::string ReadString();
    void SendBytes(std::vector<uint8_t> buffer);
    void SendText(std::string text);
    void SendStream(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    void SendErrorPage(bool showPath);
    void SendNotFound();
    void SendBadRequest();
    void SendException(std::exception &ex);
    void SendServerSentEvents(std::shared_ptr<ServerSentEvents> sse);
    std::shared_ptr<Tesses::Framework::Streams::Stream> OpenResponseStream();
    std::shared_ptr<Tesses::Framework::Streams::Stream> OpenRequestStream();
    ServerContext &WithStatusCode(StatusCode code);
    ServerContext &WithLastModified(Date::DateTime time);
    ServerContext &WithHeader(std::string key, std::string value);
    ServerContext &WithSingleHeader(std::string key, std::string value);
    ServerContext &WithMimeType(std::string mime);
    ServerContext &WithContentDisposition(std::string filename, bool isInline);
    ServerContext &WriteHeaders();
    ServerContext &WithLocationHeader(std::string url);
    ServerContext &WithLocationHeader(std::string url, StatusCode sc);
    ServerContext &
    WithHeaderIntercepter(std::function<bool(ServerContext &)> cb);
    ServerContext &WithDebug(bool debug = true);
    bool Debug();
    void StartWebSocketSession(
        std::function<void(std::function<void(WebSocketMessage &)>,
                           std::function<void()>, std::function<void()>)>
            onOpen,
        std::function<void(WebSocketMessage &)> onReceive,
        std::function<void(bool)> onClose);
    void StartWebSocketSession(std::shared_ptr<WebSocketConnection> connection);
    std::string GetServerRoot();
    std::string MakeAbsolute(std::string path);
    void SendRedirect(std::string url);
    void SendRedirect(std::string url, StatusCode sc);

    template <class T> T *GetServerContentData(std::string tag) {
        std::string name = typeid(T).name();
        name.push_back(' ');
        name.append(tag);
        if (data.count(name) > 0)
            return dynamic_cast<T *>(data[name]);
        T *item = new T();
        data[name] = item;
        return item;
    }
    friend class ServerSentEvents;
};

class IHttpServer {
  public:
    virtual bool Handle(ServerContext &ctx) = 0;
    virtual ~IHttpServer();
};

class HttpServer {
    std::shared_ptr<Tesses::Framework::Streams::TcpServer> server;
    std::shared_ptr<IHttpServer> http;
    Tesses::Framework::Threading::Thread *thrd;

    bool showIPs;
    bool showARTL;
    bool debug;

  public:
    HttpServer(std::shared_ptr<Tesses::Framework::Streams::TcpServer> tcpServer,
               std::shared_ptr<IHttpServer> http, bool showIPs = true,
               bool debug = false);
    HttpServer(uint16_t port, std::shared_ptr<IHttpServer> http,
               bool showIPs = true, bool debug = false);
    HttpServer(std::string unixPath, std::shared_ptr<IHttpServer> http,
               bool debug = false);
    uint16_t GetPort();
    void StartAccepting();
    static void
    Process(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,
            std::shared_ptr<IHttpServer> server, std::string ip, uint16_t port,
            uint16_t serverPort, bool encrypted, bool debug = false);
    ~HttpServer();
};
} // namespace Tesses::Framework::Http
