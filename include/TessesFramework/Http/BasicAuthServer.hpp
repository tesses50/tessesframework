#pragma once
#include "HttpServer.hpp"
namespace Tesses::Framework::Http {
    class BasicAuthServer : public Tesses::Framework::Http::IHttpServer
    {
        public:
            std::shared_ptr<IHttpServer> server;
            std::function<bool(std::string username, std::string password)> authorization;
            std::string realm;

            BasicAuthServer();
            BasicAuthServer(std::shared_ptr<IHttpServer> server, std::function<bool(std::string username, std::string password)> auth,std::string realm="Protected Content");
            bool Handle(ServerContext& ctx);


            static bool GetCreds(ServerContext& ctx, std::string& user, std::string& pass);
    };
}