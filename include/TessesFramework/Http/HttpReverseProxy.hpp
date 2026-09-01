/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
    https://git.tesses.org/tesses50/crosslang

    Copyright (C) 2026 Mike Nolan
    SPDX-License-Identifier: GPL-3.0-or-later WITH TessesFramework-Exception-1.0

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
#include "HttpClient.hpp"
#include "HttpServer.hpp"
#include <unordered_set>

namespace Tesses::Framework::Http {
enum class ReverseProxyAction { Continue, Handled, Unhandled };
class ReverseProxyConnectionBuilder {
  public:
    ReverseProxyConnectionBuilder(const ReverseProxyConnectionBuilder &b) =
        delete;
    ReverseProxyConnectionBuilder &
    operator=(const ReverseProxyConnectionBuilder &b) = delete;
    ReverseProxyConnectionBuilder(ReverseProxyConnectionBuilder &&b) = delete;
    ReverseProxyConnectionBuilder &
    operator=(ReverseProxyConnectionBuilder &&b) = delete;

    ReverseProxyConnectionBuilder(ServerContext &ctx,
                                  bool essentalheaders = true);
    ReverseProxyConnectionBuilder &WithHeadersFromRequest();
    ReverseProxyConnectionBuilder &WithHeader(std::string key,
                                              std::string value);
    ReverseProxyConnectionBuilder &SetHeader(std::string key,
                                             std::string value);
    ReverseProxyConnectionBuilder &WithoutHeader(std::string key);
    ReverseProxyConnectionBuilder &WithUrl(std::string url);
    ReverseProxyConnectionBuilder &WithResponseCallback(
        std::function<ReverseProxyAction(ServerContext &, HttpResponse &)> rc);
    ReverseProxyConnectionBuilder &
    WithWhitelistedUpgrade(std::string protocol = "websocket");
    bool Handle();

  private:
    ServerContext &m_ctx;
    std::optional<std::string> m_url;
    HttpDictionary m_reqheaders;
    std::unordered_set<std::string> m_whitelistedupgrades;
    std::function<ReverseProxyAction(ServerContext &, HttpResponse &)> m_rc;
    bool m_fail = false;
};

class ReverseProxyServer : public IHttpServer {
  private:
    std::string url;
    std::function<ReverseProxyAction(ServerContext &,
                                     ReverseProxyConnectionBuilder &, Uri &)>
        request_callback;
    std::function<ReverseProxyAction(ServerContext &, HttpResponse &)>
        response_callback;
    bool essentialheaders;

  public:
    ReverseProxyServer(
        std::string url,
        std::function<ReverseProxyAction(
            ServerContext &, ReverseProxyConnectionBuilder &, Uri &)>
            request_callback = nullptr,
        std::function<ReverseProxyAction(ServerContext &, HttpResponse &)>
            response_callback = nullptr,
        bool essentialheaders = true);
    bool Handle(ServerContext &ctx);
};
} // namespace Tesses::Framework::Http