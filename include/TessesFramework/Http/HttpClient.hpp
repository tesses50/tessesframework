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
#include "../Streams/Stream.hpp"
#include "HttpUtils.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/Filesystem/VFSFix.hpp"
#include "WebSocket.hpp"
namespace Tesses::Framework::Http {

class HttpRequestBody {
  public:
    virtual void HandleHeaders(HttpDictionary &dict);
    virtual void
    Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm) = 0;
    virtual ~HttpRequestBody();
};

class StreamHttpRequestBody : public HttpRequestBody {
    std::shared_ptr<Tesses::Framework::Streams::Stream> strm;

    std::string mimeType;

  public:
    StreamHttpRequestBody(
        std::shared_ptr<Tesses::Framework::Streams::Stream> strm,
        std::string mimeType);
    void HandleHeaders(HttpDictionary &dict);
    void Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    ~StreamHttpRequestBody();
};

class TextHttpRequestBody : public HttpRequestBody {
    std::string text;
    std::string mimeType;

  public:
    TextHttpRequestBody(std::string text, std::string mimeType);
    void HandleHeaders(HttpDictionary &dict);
    void Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    ~TextHttpRequestBody();
};

class HttpRequest {
  public:
    HttpRequest();
    std::string trusted_root_cert_bundle;
    bool ignoreSSLErrors;
    bool followRedirects;

    std::string method;
    std::string url;
    std::string unixSocket;
    HttpDictionary requestHeaders;
    HttpRequestBody *body;

    static std::shared_ptr<Tesses::Framework::Streams::Stream>
    EstablishConnection(Uri uri, bool ignoreSSLErrors,
                        std::string trusted_root_cert_bundle);
    static std::shared_ptr<Tesses::Framework::Streams::Stream>
    EstablishUnixPathConnection(std::string unixPath, Uri uri,
                                bool ignoreSSLErrors,
                                std::string trusted_root_cert_bundle);

    void SendRequest(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
};

class HttpResponse {
  private:
    bool owns;
    std::shared_ptr<Tesses::Framework::Streams::Stream> handleStrm;

  public:
    HttpResponse(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    HttpResponse(HttpRequest &request);
    std::string version;
    StatusCode statusCode;
    HttpDictionary responseHeaders;
    std::string ReadAsString();
    std::shared_ptr<Tesses::Framework::Streams::Stream> ReadAsStream();
    void CopyToStream(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    std::shared_ptr<Tesses::Framework::Streams::Stream> GetInternalStream();
    ~HttpResponse();
};

void DownloadToStreamSimple(
    std::string url, std::shared_ptr<Tesses::Framework::Streams::Stream> strm);

void DownloadToFileSimple(
    std::string url, std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,
    Tesses::Framework::Filesystem::VFSPath path);

void DownloadToFileSimple(std::string url,
                          Tesses::Framework::Filesystem::VFSPath path);
std::string DownloadToStringSimple(std::string url);

bool WebSocketClientSuccessDefault(HttpDictionary &dict, bool v);
void WebSocketClient(std::string url, HttpDictionary &requestHeaders,
                     std::shared_ptr<WebSocketConnection> wsc,
                     std::function<bool(HttpDictionary &, bool)> cb =
                         WebSocketClientSuccessDefault);

void DownloadUnixSocketToStreamSimple(
    std::string unixSocket, std::string url,
    std::shared_ptr<Tesses::Framework::Streams::Stream> strm);

void DownloadUnixSocketToFileSimple(
    std::string unixSocket, std::string url,
    std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,
    Tesses::Framework::Filesystem::VFSPath path);

void DownloadUnixSocketToFileSimple(
    std::string unixSocket, std::string url,
    Tesses::Framework::Filesystem::VFSPath path);
std::string DownloadUnixSocketToStringSimple(std::string unixSocket,
                                             std::string url);

void WebSocketUnixSocketClient(std::string unixSocket, std::string url,
                               HttpDictionary &requestHeaders,
                               std::shared_ptr<WebSocketConnection> wsc,
                               std::function<bool(HttpDictionary &, bool)> cb =
                                   WebSocketClientSuccessDefault);
} // namespace Tesses::Framework::Http