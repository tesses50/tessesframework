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
#include "../Filesystem/VFS.hpp"
#include "../Filesystem/VFSFix.hpp"
#include "HttpServer.hpp"

namespace Tesses::Framework::Http {
using ServerRequestHandler = std::function<bool(ServerContext &)>;

class RouteServer : public IHttpServer {
    class RouteServerRoute {
      public:
        std::vector<std::pair<std::string, bool>> parts;
        std::string method;
        ServerRequestHandler handler;

        RouteServerRoute() = default;
        RouteServerRoute(std::string route, std::string method,
                         ServerRequestHandler handler);
        bool Equals(Tesses::Framework::Filesystem::VFSPath &path,
                    HttpDictionary &args);
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
    void Add(std::string method, std::string pattern,
             ServerRequestHandler handler);
    bool Handle(ServerContext &ctx);
};
} // namespace Tesses::Framework::Http