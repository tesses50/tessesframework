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
#include "HttpServer.hpp"
namespace Tesses::Framework::Http {
class BasicAuthServer : public Tesses::Framework::Http::IHttpServer {
  public:
    std::shared_ptr<IHttpServer> server;
    std::function<bool(std::string username, std::string password)>
        authorization;
    std::string realm;

    BasicAuthServer();
    BasicAuthServer(
        std::shared_ptr<IHttpServer> server,
        std::function<bool(std::string username, std::string password)> auth,
        std::string realm = "Protected Content");
    bool Handle(ServerContext &ctx);

    static bool GetCreds(ServerContext &ctx, std::string &user,
                         std::string &pass);
};
} // namespace Tesses::Framework::Http