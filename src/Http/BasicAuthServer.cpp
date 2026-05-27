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

#include "TessesFramework/Http/BasicAuthServer.hpp"
#include "TessesFramework/Crypto/Crypto.hpp"
#include <iostream>
namespace Tesses::Framework::Http {

BasicAuthServer::BasicAuthServer() {}
BasicAuthServer::BasicAuthServer(
    std::shared_ptr<IHttpServer> server,
    std::function<bool(std::string username, std::string password)> auth,
    std::string realm)
    : server(server), authorization(auth), realm(realm) {}
bool BasicAuthServer::Handle(ServerContext &ctx) {
    std::string www_authenticate =
        "Basic realm=\"" + this->realm + "\", charset=\"UTF-8\"";
    std::string user;
    std::string pass;
    if (!GetCreds(ctx, user, pass) || !this->authorization(user, pass)) {
        ctx.responseHeaders.SetValue("WWW-Authenticate", www_authenticate);
        ctx.statusCode = Unauthorized;
        ctx.WithMimeType("text/html").SendText("<h1>Unauthorized</h1>");
        return true;
    }

    if (this->server)
        return this->server->Handle(ctx);
    ctx.statusCode = InternalServerError;
    ctx.WithMimeType("text/html")
        .SendText("<h1>Internal Server Error</h1>\r\n<h3>REASON: Internal "
                  "server not set on Basic Auth</h3>");
    return true;
}

bool BasicAuthServer::GetCreds(ServerContext &ctx, std::string &user,
                               std::string &pass) {
    std::string auth;
    if (!ctx.requestHeaders.TryGetFirst("Authorization", auth))
        return false;

    auto security = HttpUtils::SplitString(auth, " ", 2);
    if (security.size() < 2)
        return false;
    if (security[0] != "Basic")
        return false;

    auto decoded = Crypto::Base64_Decode(security[1]);

    std::string decoded_str(decoded.begin(), decoded.end());
    security = HttpUtils::SplitString(decoded_str, ":", 2);
    if (security.size() < 2)
        return false;
    user = security[0];
    pass = security[1];
    return true;
}
} // namespace Tesses::Framework::Http