#pragma once
#include "HttpServer.hpp"
#include "../Filesystem/VFSFix.hpp"
#include "../Filesystem/VFS.hpp"

namespace Tesses::Framework::Http
{
    using ServerRequestHandler = std::function<bool(ServerContext&)>;
    
    
    class RouteServer : public IHttpServer
    {
        class RouteServerRoute {
        public:
            std::vector<std::pair<std::string,bool>> parts;
            std::string method;
            ServerRequestHandler handler;

            RouteServerRoute() = default;
            RouteServerRoute(std::string route, std::string method, ServerRequestHandler handler);
            bool Equals(Tesses::Framework::Filesystem::VFSPath& path, HttpDictionary& args);
        };
        std::vector<RouteServerRoute> routes;
        std::shared_ptr<IHttpServer> root;
        public:
            RouteServer() = default;
            RouteServer(std::shared_ptr<IHttpServer> root);
            void Get(std::string pattern, ServerRequestHandler handler);
            void Post(std::string pattern, ServerRequestHandler handler);
            void Put(std::string pattern, ServerRequestHandler handler);
            void Patch(std::string pattern, ServerRequestHandler handler);

            void Delete(std::string pattern, ServerRequestHandler handler);

            void Trace(std::string pattern, ServerRequestHandler handler);  
            void Options(std::string pattern, ServerRequestHandler handler);
            void Add(std::string method, std::string pattern, ServerRequestHandler handler);
            bool Handle(ServerContext& ctx);
            
    };
}