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
#include "TessesFramework/Http/HttpReverseProxy.hpp"
namespace Tesses::Framework::Http {

static std::string CONNECTION = "CONNECTION";
static std::string UPGRADE = "UPGRADE";

static std::initializer_list<std::string> STRIP_EQ = {
    "XREALIP",     "FORWARDED",        "KEEPALIVE", "TE",
    "TRAILER",     "TRANSFERENCODING", "EXPECT",    "CONTENTLENGTH",
    "CONTENTTYPE", "ACCEPTENCODING",   "HOST"}; // Content-Type is OK but we get
                                                // it elsewhere, Content-Length
                                                // is the same way

static std::initializer_list<std::string> STRIP_PREFIX = {
    "XFORWARDED", "PROXY", "XORIGINAL", "CF", "SSL", "XSSL"};

static void sanitise_uppercase_only(const std::string &src, std::string &dest) {
    dest.reserve(src.size());
    for (size_t i = 0; i < src.size(); i++) {
        char c = (src[i] & 0b11011111);
        if (c >= 'A' && c <= 'Z')
            dest.push_back(c);
    }
}

static void get_upgrades(const std::string &src,
                         std::unordered_set<std::string> &upgrades) {
    auto parts = HttpUtils::SplitString(src, ",");
    for (auto &item : parts) {
        size_t start = item.find_first_not_of(" \t");
        if (start == std::string::npos)
            continue; // Empty
        size_t end = item.find_last_not_of(" \t");
        upgrades.insert(
            HttpUtils::ToLower(item.substr(start, end - start + 1)));
    }
}

static bool uppercase_eq(const std::string &src, const std::string &match,
                         bool startsWith) {
    std::string cmp;
    sanitise_uppercase_only(src, cmp);

    if (startsWith) {
        if (cmp.size() < match.size())
            return false;
        for (size_t i = 0; i < match.size(); i++) {
            if (cmp[i] != match[i])
                return false;
        }
        return true;
    } else {
        return cmp == match;
    }
}

ReverseProxyConnectionBuilder::ReverseProxyConnectionBuilder(
    ServerContext &ctx, bool essentalheaders)
    : m_reqheaders(false), m_ctx(ctx) {

    if (essentalheaders) {
        std::string str;

        if (ctx.requestHeaders.TryGetFirst("Host", str))
            m_reqheaders.SetValue("X-Forwarded-Host", str);

        m_reqheaders.SetValue("X-Forwarded-For", ctx.ip);

        m_reqheaders.SetValue("X-Forwarded-Proto",
                              ctx.encrypted ? "https" : "http");
    }
}
ReverseProxyConnectionBuilder &
ReverseProxyConnectionBuilder::WithHeadersFromRequest() {
    std::unordered_set<std::string> wanted_upgrade_protocols;
    bool doUpgradeIfApprovedUpgrade = false;

    for (auto item : this->m_ctx.requestHeaders.kvp) {

        if (!item.first.empty()) {

            if (uppercase_eq(item.first, UPGRADE, false)) {
                for (auto &item2 : item.second)
                    get_upgrades(item2, wanted_upgrade_protocols);

                continue;
            }

            if (uppercase_eq(item.first, CONNECTION, false)) {
                bool anyNotUpgrade = item.second.size() != 1;

                if (!anyNotUpgrade &&
                    !uppercase_eq(item.second.front(), UPGRADE, false)) {
                    anyNotUpgrade = true;
                    continue;
                }

                if (anyNotUpgrade)
                    continue;

                doUpgradeIfApprovedUpgrade = true;

                continue;
            }
            bool doCont = false;
            for (auto &strip : STRIP_EQ)
                if (uppercase_eq(item.first, strip, false)) {
                    doCont = true;
                    break;
                }
            if (doCont)
                continue;
            for (auto &strip : STRIP_PREFIX)
                if (uppercase_eq(item.first, strip, true)) {
                    doCont = true;
                    break;
                }

            if (doCont)
                continue;

            this->m_reqheaders.SetValue(item.first, item.second);
        }
    }

    if (doUpgradeIfApprovedUpgrade && !wanted_upgrade_protocols.empty()) {
        std::vector<std::string> valid_upgrades;
        for (auto &item : this->m_whitelistedupgrades) {
            if (wanted_upgrade_protocols.find(item) !=
                wanted_upgrade_protocols.end()) {
                valid_upgrades.push_back(item);
            }
        }

        if (!valid_upgrades.empty()) {
            this->m_reqheaders.SetValue("Connection", "upgrade");

            std::string upgrade_text = "";
            for (auto &item : valid_upgrades) {
                if (upgrade_text.empty())
                    upgrade_text = item;
                else
                    upgrade_text += ", " + item;
            }

            this->m_reqheaders.SetValue("Upgrade", upgrade_text);
        }
    }

    return *this;
}
ReverseProxyConnectionBuilder &
ReverseProxyConnectionBuilder::WithHeader(std::string key, std::string value) {
    this->m_reqheaders.AddValue(key, value);
    return *this;
}
ReverseProxyConnectionBuilder &
ReverseProxyConnectionBuilder::SetHeader(std::string key, std::string value) {
    this->m_reqheaders.SetValue(key, value);
    return *this;
}
ReverseProxyConnectionBuilder &
ReverseProxyConnectionBuilder::WithoutHeader(std::string key) {
    this->m_reqheaders.Clear(key, false);
    return *this;
}
ReverseProxyConnectionBuilder &
ReverseProxyConnectionBuilder::WithUrl(std::string url) {
    this->m_url = url;
    return *this;
}
ReverseProxyConnectionBuilder &
ReverseProxyConnectionBuilder::WithResponseCallback(
    std::function<ReverseProxyAction(ServerContext &, HttpResponse &)> rc) {
    this->m_rc = rc;
    return *this;
}

static void
StreamExchange(std::shared_ptr<Tesses::Framework::Streams::Stream> server,
               std::shared_ptr<Tesses::Framework::Streams::Stream> client) {
    Tesses::Framework::Threading::Thread server2client([server,
                                                        client]() -> void {
        try {
            std::vector<uint8_t> buffer;
            buffer.resize(1024);

            size_t read = 0;

            do {
                read = server->Read(buffer.data(), buffer.size());
                if (read == 0)
                    return;
                read = client->Write(buffer.data(),
                                     std::min<size_t>(read, buffer.size()));
            } while (read != 0);
        } catch (...) {
        }

        server->Shutdown(Tesses::Framework::Streams::StreamShutdownMode::Read);

        client->Shutdown(Tesses::Framework::Streams::StreamShutdownMode::Write);
    });

    Tesses::Framework::Threading::Thread client2server([client,
                                                        server]() -> void {
        try {
            std::vector<uint8_t> buffer;
            buffer.resize(1024);

            size_t read = 0;

            do {
                read = client->Read(buffer.data(), buffer.size());
                if (read == 0)
                    return;
                read = server->Write(buffer.data(),
                                     std::min<size_t>(read, buffer.size()));
            } while (read != 0);
        } catch (...) {
        }
        client->Shutdown(Tesses::Framework::Streams::StreamShutdownMode::Read);

        server->Shutdown(Tesses::Framework::Streams::StreamShutdownMode::Write);
    });

    server2client.Join();
    client2server.Join();
}

bool ReverseProxyConnectionBuilder::Handle() {
    if (m_fail) {
        m_ctx.statusCode = StatusCode::BadRequest;
        return false;
    }

    if (!m_url) {
        m_ctx.statusCode = StatusCode::InternalServerError;
        m_ctx.SendText("<!DOCTYPE html>"
                       "<html>"
                       "<head><meta name=\"color-scheme\" "
                       "content=\"dark light\"><title>500 Internal Server "
                       "Error</title></head>"
                       "<body>"
                       "<h1>500 Internal Server Error</h1><p>" +
                       HttpUtils::HtmlEncode(
                           "Please chain the reverse proxy url with "
                           ".WithUrl(\"http://example.com/path/on/backend\")") +
                       "</p>"
                       "</body>"
                       "</html>"

        );

        return true;
    }

    HttpRequest req;

    req.url = m_url.value();
    req.method = m_ctx.method;
    req.requestHeaders = m_reqheaders;
    req.followRedirects = false;
    std::string type;

    bool isNotUpgradeOrGet =
        !(m_ctx.method == "GET" || m_ctx.method == "HEAD" ||
          m_reqheaders.AnyEquals("Connection", "upgrade"));

    if (isNotUpgradeOrGet &&
        m_ctx.requestHeaders.TryGetFirst("Content-Type", type)) {
        if (type == "application/x-www-form-urlencoded") {
            std::string body = HttpUtils::QueryParamsEncode(m_ctx.bodyParams);
            req.body = std::make_shared<TextHttpRequestBody>(
                body, "application/x-www-form-urlencoded");
        } else {
            req.body = std::make_shared<StreamHttpRequestBody>(
                m_ctx.OpenRequestStream(), type);
        }
    } else if (isNotUpgradeOrGet) {
        req.body = std::make_shared<StreamHttpRequestBody>(
            m_ctx.OpenRequestStream(), "application/octet-stream");
    }
    std::unique_ptr<HttpResponse> resp;

    try {
        resp = std::make_unique<HttpResponse>(req);
    } catch (...) {
        m_ctx.statusCode = StatusCode::BadGateway;
        m_ctx.SendErrorPage(true);
        return true;
    }

    std::string val;

    if (resp->responseHeaders.TryGetOnlyOne("Content-Type", val))
        this->m_ctx.responseHeaders.SetValue("Content-Type", val);
    if (resp->responseHeaders.TryGetOnlyOne("Set-Cookie", val))
        this->m_ctx.responseHeaders.SetValue("Set-Cookie", val);
    if (resp->responseHeaders.TryGetOnlyOne("Cache-Control", val))
        this->m_ctx.responseHeaders.SetValue("Cache-Control", val);
    if (resp->responseHeaders.TryGetOnlyOne("ETag", val))
        this->m_ctx.responseHeaders.SetValue("ETag", val);
    if (resp->responseHeaders.TryGetOnlyOne("WWW-Authenticate", val))
        this->m_ctx.responseHeaders.SetValue("WWW-Authenticate", val);
    if (resp->responseHeaders.TryGetOnlyOne("Last-Modified", val))
        this->m_ctx.responseHeaders.SetValue("Last-Modified", val);
    if (resp->responseHeaders.TryGetOnlyOne("Date", val))
        this->m_ctx.responseHeaders.SetValue("Date", val);

    m_ctx.statusCode = resp->statusCode;

    std::unordered_set<std::string> wanted_upgrade_protocols;
    bool doUpgradeIfApprovedUpgrade = false;

    for (auto &item : resp->responseHeaders.kvp) {
        if (!item.first.empty()) {
            if (uppercase_eq(item.first, UPGRADE, false)) {
                for (auto &item2 : item.second)
                    get_upgrades(item2, wanted_upgrade_protocols);

                continue;
            }

            if (uppercase_eq(item.first, CONNECTION, false)) {
                bool anyNotUpgrade = item.second.size() != 1;

                if (!anyNotUpgrade &&
                    !uppercase_eq(item.second.front(), UPGRADE, false)) {
                    anyNotUpgrade = true;
                    continue;
                }

                if (anyNotUpgrade)
                    continue;

                doUpgradeIfApprovedUpgrade = true;

                continue;
            }
        }
    }

    if (resp->responseHeaders.TryGetOnlyOne("Location", val)) {
        Uri uri_src;
        if (Uri::TryParse(m_url.value(), uri_src)) {
            Uri uri_dest;
            if (uri_src.Relative(val, uri_dest)) {

                if (uri_dest.host == uri_src.host &&
                    uri_dest.port == uri_src.port) {
                    Tesses::Framework::Filesystem::VFSPath dest = uri_dest.path;
                    Tesses::Framework::Filesystem::VFSPath src = uri_src.path;

                    auto path = dest.MakeRelative(src);

                    auto path2 = path.MakeAbsolute(m_ctx.path);

                    std::vector<std::string> parts;

                    for (auto item : path2.path) {
                        if (item == "..") {
                            if (!parts.empty()) {
                                parts.erase(parts.end() - 1);
                            } else {
                                m_ctx.statusCode =
                                    StatusCode::InternalServerError;
                                m_ctx.SendText(
                                    "<!DOCTYPE html>"
                                    "<html>"
                                    "<head><meta name=\"color-scheme\" "
                                    "content=\"dark light\"><title>500 "
                                    "Internal Server "
                                    "Error</title></head>"
                                    "<body>"
                                    "<h1>500 Internal Server Error</h1><p>"
                                    "A redirect went beyond the reverse "
                                    "proxy "
                                    "root"
                                    "</p>"
                                    "</body>"
                                    "</html>"

                                );
                                return true;
                            }
                        } else if (item == ".") {
                            // do nothing but don't emit this
                        } else {
                            parts.push_back(item);
                        }
                    }
                    Tesses::Framework::Filesystem::VFSPath newpath =
                        m_ctx.GetServerRoot();

                    newpath.path.insert(newpath.path.cend(), parts.cbegin(),
                                        parts.cend());

                    std::string location =
                        newpath.ToString() + uri_dest.GetQuery();
                    if (!uri_dest.hash.empty())
                        location += "#" + uri_dest.hash;
                    this->m_ctx.responseHeaders.SetValue("Location", location);
                } else {
                    this->m_ctx.responseHeaders.SetValue("Location",
                                                         uri_dest.ToString());
                }
            }
        }
    }

    if (this->m_rc)
        switch (this->m_rc(m_ctx, *resp)) {
        case ReverseProxyAction::Continue:
            break;
        case ReverseProxyAction::Handled:
            return true;
        case ReverseProxyAction::Unhandled:
            return false;
        }

    if (resp->statusCode == StatusCode::SwitchingProtocols) {
        if (doUpgradeIfApprovedUpgrade && !wanted_upgrade_protocols.empty()) {
            std::vector<std::string> valid_upgrades;
            for (auto &item : this->m_whitelistedupgrades) {
                if (wanted_upgrade_protocols.find(item) !=
                    wanted_upgrade_protocols.end()) {
                    valid_upgrades.push_back(item);
                }
            }

            if (!valid_upgrades.empty()) {
                m_ctx.responseHeaders.SetValue("Connection", "upgrade");

                std::string upgrade_text = "";
                for (auto &item : valid_upgrades) {
                    if (upgrade_text.empty())
                        upgrade_text = item;
                    else
                        upgrade_text += ", " + item;
                }

                m_ctx.responseHeaders.SetValue("Upgrade", upgrade_text);

                m_ctx.WriteHeaders();

                StreamExchange(m_ctx.GetStream(), resp->GetInternalStream());
                return true;
            }
        }
        m_ctx.statusCode = StatusCode::BadRequest;
        m_ctx.SendErrorPage(true);
        return true;
    } else {
        m_ctx.SendStream(resp->ReadAsStream());
        return true;
    }
}

ReverseProxyServer::ReverseProxyServer(
    std::string url,
    std::function<ReverseProxyAction(ServerContext &,
                                     ReverseProxyConnectionBuilder &, Uri &)>
        request_callback,
    std::function<ReverseProxyAction(ServerContext &, HttpResponse &)>
        response_callback,
    bool essentialheaders)
    : url(url), request_callback(request_callback),
      response_callback(response_callback), essentialheaders(essentialheaders) {
}
bool ReverseProxyServer::Handle(ServerContext &ctx) {
    Uri uri;

    if (Uri::TryParse(url, uri)) {
        uri.query = ctx.queryParams;
        Tesses::Framework::Filesystem::VFSPath srcUrl = uri.path;
        Tesses::Framework::Filesystem::VFSPath destUrl = ctx.path;

        srcUrl.path.insert(srcUrl.path.cend(), destUrl.path.cbegin(),
                           destUrl.path.cend());
        uri.path = srcUrl.ToString();

        ReverseProxyConnectionBuilder builder(ctx, essentialheaders);

        if (request_callback) {
            switch (request_callback(ctx, builder, uri)) {
            case ReverseProxyAction::Continue:
                return builder.WithResponseCallback(response_callback).Handle();
                break;
            case ReverseProxyAction::Unhandled:
                return false;
            case ReverseProxyAction::Handled:
                return true;
            }
        } else {
            return builder.WithUrl(uri.ToString())
                .WithHeadersFromRequest()
                .WithResponseCallback(response_callback)
                .Handle();
        }
    }
    return false;
}

} // namespace Tesses::Framework::Http