#pragma once
#include "HttpServer.hpp"

namespace Tesses::Framework::Http
{
    class ChangeableServer {
        public:
            ChangeableServer();
            ChangeableServer(std::shared_ptr<IHttpServer> original);
            std::shared_ptr<IHttpServer> server;
            bool Handle(ServerContext& ctx);
            ~ChangeableServer();
    };
}