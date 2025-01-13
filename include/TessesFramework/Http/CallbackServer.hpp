#pragma once
#include "HttpServer.hpp"

namespace Tesses::Framework::Http
{
    class CallbackServer : public IHttpServer
    {
        std::function<bool(ServerContext&)> cb;
        std::function<void()> destroy;
        public:
            CallbackServer(std::function<bool(ServerContext&)> cb);
            CallbackServer(std::function<bool(ServerContext&)> cb,std::function<void()> destroy);
            bool Handle(ServerContext& ctx);
            ~CallbackServer();
    };
}