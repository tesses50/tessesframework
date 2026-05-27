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

#include "TessesFramework/Http/HttpClient.hpp"
#include "TessesFramework/Crypto/ClientTLSStream.hpp"
#include "TessesFramework/Crypto/Crypto.hpp"
#include "TessesFramework/Http/HttpStream.hpp"
#include "TessesFramework/Streams/BufferedStream.hpp"
#include "TessesFramework/Streams/NetworkStream.hpp"
#include "TessesFramework/TextStreams/StreamReader.hpp"
#include "TessesFramework/TextStreams/StreamWriter.hpp"
#include "TessesFramework/Threading/Mutex.hpp"
#include "TessesFramework/Threading/Thread.hpp"

#include <iostream>
#include <string>
using Stream = Tesses::Framework::Streams::Stream;
using NetworkStream = Tesses::Framework::Streams::NetworkStream;
using ClientTLSStream = Tesses::Framework::Crypto::ClientTLSStream;
using StreamReader = Tesses::Framework::TextStreams::StreamReader;
using StreamWriter = Tesses::Framework::TextStreams::StreamWriter;
using BufferedStream = Tesses::Framework::Streams::BufferedStream;
using HttpStream = Tesses::Framework::Http::HttpStream;
namespace Tesses::Framework::Http {

void HttpRequestBody::HandleHeaders(HttpDictionary &dict) {}

HttpRequestBody::~HttpRequestBody() {}
TextHttpRequestBody::TextHttpRequestBody(std::string text,
                                         std::string mimeType) {
    this->text = text;
    this->mimeType = mimeType;
}
void TextHttpRequestBody::HandleHeaders(HttpDictionary &dict) {
    dict.SetValue("Content-Type", this->mimeType);
    dict.SetValue("Content-Length", std::to_string(this->text.size()));
}
void TextHttpRequestBody::Write(
    std::shared_ptr<Tesses::Framework::Streams::Stream> strm) {
    strm->WriteBlock((const uint8_t *)this->text.c_str(), this->text.size());
}
TextHttpRequestBody::~TextHttpRequestBody() {}

StreamHttpRequestBody::StreamHttpRequestBody(std::shared_ptr<Stream> strm,
                                             std::string mimeType) {
    this->strm = strm;
    this->mimeType = mimeType;
}
void StreamHttpRequestBody::HandleHeaders(HttpDictionary &dict) {
    dict.AddValue("Content-Type", this->mimeType);
    auto len = this->strm->GetLength();
    if (len > -1)
        dict.AddValue("Content-Length", std::to_string(len));
}
void StreamHttpRequestBody::Write(
    std::shared_ptr<Tesses::Framework::Streams::Stream> strm) {
    this->strm->CopyTo(strm);
}
StreamHttpRequestBody::~StreamHttpRequestBody() {}
HttpRequest::HttpRequest() {
    this->body = nullptr;
    this->followRedirects = true;
    this->ignoreSSLErrors = false;
    this->trusted_root_cert_bundle = "";
    this->method = "GET";
    this->requestHeaders.SetValue("Connection", "close");
}

void HttpRequest::SendRequest(
    std::shared_ptr<Tesses::Framework::Streams::Stream> strm) {
    Uri uri;
    if (!Uri::TryParse(this->url, uri))
        return;

    if (body != nullptr) {
        body->HandleHeaders(requestHeaders);
    }

    std::string request = method + " " + uri.GetPathAndQuery() +
                          " HTTP/1.1\r\nHost: " + uri.HostPort() + "\r\n";
    for (auto headers : requestHeaders.kvp) {
        for (auto item : headers.second) {
            request.append(headers.first);
            request.append(": ");
            request.append(item);
            request.append("\r\n");
        }
    }

    request.append("\r\n");

    StreamWriter writer(strm);
    writer.Write(request);

    if (body != nullptr) {
        body->Write(strm);
    }
}
std::shared_ptr<Stream>
HttpRequest::EstablishConnection(Uri uri, bool ignoreSSLErrors,
                                 std::string trusted_root_cert_bundle) {
    if (uri.scheme == "http:" || uri.scheme == "ws:") {
        return std::make_shared<NetworkStream>(uri.host, uri.GetPort(), false,
                                               false, false);
    } else if (uri.scheme == "https:" || uri.scheme == "wss:") {
        auto netStrm = std::make_shared<NetworkStream>(uri.host, uri.GetPort(),
                                                       false, false, false);
        if (netStrm == nullptr)
            return nullptr;
        return std::make_shared<ClientTLSStream>(
            netStrm, !ignoreSSLErrors, uri.host, trusted_root_cert_bundle);
    }

    return nullptr;
}
std::shared_ptr<Stream>
HttpRequest::EstablishUnixPathConnection(std::string unixPath, Uri uri,
                                         bool ignoreSSLErrors,
                                         std::string trusted_root_cert_bundle) {
    if (uri.scheme == "http:" || uri.scheme == "ws:") {
        return std::make_shared<NetworkStream>(unixPath, false);
    } else if (uri.scheme == "https:" || uri.scheme == "wss:") {
        auto netStrm = std::make_shared<NetworkStream>(unixPath, false);
        if (netStrm == nullptr)
            return nullptr;
        return std::make_shared<ClientTLSStream>(
            netStrm, !ignoreSSLErrors, uri.host, trusted_root_cert_bundle);
    }

    return nullptr;
}
std::shared_ptr<Tesses::Framework::Streams::Stream>
HttpResponse::GetInternalStream() {
    return this->handleStrm;
}

HttpResponse::~HttpResponse() {}

HttpResponse::HttpResponse(std::shared_ptr<Stream> strm) {
    this->handleStrm = nullptr;
    StreamReader reader(strm);
    std::string statusLine;
    if (!reader.ReadLineHttp(statusLine))
        return;
    auto statusLinesPart = HttpUtils::SplitString(statusLine, " ", 3);
    if (statusLinesPart.size() >= 2) {
        this->version = statusLinesPart[0];
        this->statusCode = (StatusCode)std::stoi(statusLinesPart[1]);
    }
    std::string line;
    while (reader.ReadLineHttp(line)) {
        if (line.empty())
            break;
        auto v = HttpUtils::SplitString(line, ": ", 2);
        if (v.size() != 2) {
            return;
        }
        this->responseHeaders.AddValue(v[0], v[1]);
        line.clear();
    }
    this->handleStrm = strm;
}

HttpResponse::HttpResponse(HttpRequest &req) {
    this->handleStrm = nullptr;

    std::string url = req.url;
    Uri uri;
    while (Uri::TryParse(url, uri)) {
        auto strm =
            req.unixSocket.empty()
                ? HttpRequest::EstablishConnection(uri, req.ignoreSSLErrors,
                                                   req.trusted_root_cert_bundle)
                : HttpRequest::EstablishUnixPathConnection(
                      req.unixSocket, uri, req.ignoreSSLErrors,
                      req.trusted_root_cert_bundle);
        if (strm == nullptr)
            return;
        auto reqHeaders = req.requestHeaders;

        if (req.body != nullptr) {
            req.body->HandleHeaders(reqHeaders);
        }

        std::string request = req.method + " " + uri.GetPathAndQuery() +
                              " HTTP/1.1\r\nHost: " + uri.HostPort() + "\r\n";

        for (auto headers : reqHeaders.kvp) {
            for (auto item : headers.second) {
                request.append(headers.first);
                request.append(": ");
                request.append(item);
                request.append("\r\n");
            }
        }

        request.append("\r\n");

        StreamWriter writer(strm);
        writer.Write(request);

        if (req.body != nullptr) {
            req.body->Write(strm);
        }

        StreamReader reader(strm);
        std::string statusLine;
        if (!reader.ReadLineHttp(statusLine))
            break;
        auto statusLinesPart = HttpUtils::SplitString(statusLine, " ", 3);
        if (statusLinesPart.size() >= 2) {
            this->version = statusLinesPart[0];
            this->statusCode = (StatusCode)std::stoi(statusLinesPart[1]);
        }
        std::string line;
        while (reader.ReadLineHttp(line)) {
            if (line.empty())
                break;
            auto v = HttpUtils::SplitString(line, ": ", 2);
            if (v.size() != 2) {
                return;
            }
            this->responseHeaders.AddValue(v[0], v[1]);
            line.clear();
        }

        std::string location;
        Uri uri2;

        if (req.followRedirects &&
            (this->statusCode == MovedPermanently ||
             this->statusCode == PermanentRedirect ||
             this->statusCode == TemporaryRedirect ||
             this->statusCode == SeeOther || this->statusCode == Found) &&
            this->responseHeaders.TryGetFirst("Location", location) &&
            uri.Relative(location, uri2)) {
            this->responseHeaders.Clear();

            url = uri2.ToString();

            continue;
        }
        this->handleStrm = strm;
        break;
    }
}
std::string HttpResponse::ReadAsString() {
    auto strm = ReadAsStream();
    if (strm == nullptr)
        return {};
    StreamReader r(strm);
    return r.ReadToEnd();
}
void HttpResponse::CopyToStream(std::shared_ptr<Stream> strm) {
    if (strm == nullptr)
        return;
    auto src = ReadAsStream();
    if (src == nullptr)
        return;
    src->CopyTo(strm);
}
std::shared_ptr<Stream> HttpResponse::ReadAsStream() {
    if (this->handleStrm == nullptr)
        return nullptr;
    int64_t length = -1;
    if (!this->responseHeaders.TryGetFirstInt("Content-Length", length)) {
        length = -1;
    }

    return std::make_shared<HttpStream>(this->handleStrm, length, true,
                                        version == "HTTP/1.1");
}

void DownloadUnixSocketToStreamSimple(
    std::string unixSocket, std::string url,
    std::shared_ptr<Tesses::Framework::Streams::Stream> strm) {
    if (strm == nullptr)
        throw std::runtime_error("strm is null");
    HttpRequest request;
    request.url = url;
    request.unixSocket = unixSocket;
    request.followRedirects = true;
    request.method = "GET";
    HttpResponse response(request);
    if (response.statusCode < 200 || response.statusCode > 299)
        throw std::runtime_error(
            "Status code does not indicate success: " +
            std::to_string(response.statusCode) + " " +
            HttpUtils::StatusCodeString(response.statusCode));
    response.CopyToStream(strm);
}

void DownloadUnixSocketToFileSimple(
    std::string unixSocket, std::string url,
    std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,
    Tesses::Framework::Filesystem::VFSPath path) {
    if (vfs == nullptr)
        throw std::runtime_error("vfs is null");
    auto strm = vfs->OpenFile(path, "wb");
    if (strm == nullptr)
        throw std::runtime_error("strm is null");
    DownloadUnixSocketToStreamSimple(unixSocket, url, strm);
}

void DownloadUnixSocketToFileSimple(
    std::string unixSocket, std::string url,
    Tesses::Framework::Filesystem::VFSPath path) {
    DownloadUnixSocketToFileSimple(
        unixSocket, url, Tesses::Framework::Filesystem::LocalFS, path);
}
std::string DownloadUnixSocketToStringSimple(std::string unixSocket,
                                             std::string url) {
    HttpRequest request;
    request.url = url;
    request.unixSocket = unixSocket;
    request.followRedirects = true;
    request.method = "GET";
    HttpResponse response(request);
    if (response.statusCode < 200 || response.statusCode > 299)
        throw std::runtime_error(
            "Status code does not indicate success: " +
            std::to_string(response.statusCode) + " " +
            HttpUtils::StatusCodeString(response.statusCode));
    return response.ReadAsString();
}
void DownloadToStreamSimple(
    std::string url, std::shared_ptr<Tesses::Framework::Streams::Stream> strm) {
    DownloadUnixSocketToStreamSimple("", url, strm);
}

void DownloadToFileSimple(
    std::string url, std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,
    Tesses::Framework::Filesystem::VFSPath path) {
    DownloadUnixSocketToFileSimple("", url, vfs, path);
}

void DownloadToFileSimple(std::string url,
                          Tesses::Framework::Filesystem::VFSPath path) {
    DownloadUnixSocketToFileSimple("", url, path);
}
std::string DownloadToStringSimple(std::string url) {
    HttpRequest request;
    request.url = url;
    request.followRedirects = true;
    request.method = "GET";
    HttpResponse response(request);
    if (response.statusCode < 200 || response.statusCode > 299)
        throw std::runtime_error(
            "Status code does not indicate success: " +
            std::to_string(response.statusCode) + " " +
            HttpUtils::StatusCodeString(response.statusCode));
    return response.ReadAsString();
}
bool WebSocketClientSuccessDefault(HttpDictionary &dict, bool v) {
    return true;
}

class WSClient {
  public:
    std::atomic<bool> closed;
    Threading::Mutex mtx;

    std::shared_ptr<WebSocketConnection> conn;
    std::shared_ptr<Stream> strm;
    void close() {
        mtx.Lock();
        this->closed = true;
        uint8_t finField = 0b10000000;
        uint8_t firstByte = finField | 0x8;
        strm->WriteByte(firstByte);
        strm->WriteByte(0);

        mtx.Unlock();
    }
    void write_len_bytes(uint64_t len) {
        if (len < 126) {
            strm->WriteByte((uint8_t)len | (uint8_t)128U);
        } else if (len < 65535) {
            uint8_t b[3];
            b[0] = 254;
            b[1] = (uint8_t)(len >> 8);
            b[2] = (uint8_t)len;

            strm->WriteBlock(b, sizeof(b));
        } else {

            uint8_t b[9];
            b[0] = 255;

            b[1] = (uint8_t)(len >> 56);
            b[2] = (uint8_t)(len >> 48);
            b[3] = (uint8_t)(len >> 40);
            b[4] = (uint8_t)(len >> 32);
            b[5] = (uint8_t)(len >> 24);
            b[6] = (uint8_t)(len >> 16);
            b[7] = (uint8_t)(len >> 8);
            b[8] = (uint8_t)len;

            strm->WriteBlock(b, sizeof(b));
        }
    }
    uint64_t get_long() {
        uint8_t buff[8];
        if (strm->ReadBlock(buff, sizeof(buff)) != sizeof(buff))
            return 0;

        uint64_t v = 0;
        v |= (uint64_t)buff[0] << 56;
        v |= (uint64_t)buff[1] << 48;
        v |= (uint64_t)buff[2] << 40;
        v |= (uint64_t)buff[3] << 32;
        v |= (uint64_t)buff[4] << 24;
        v |= (uint64_t)buff[5] << 16;
        v |= (uint64_t)buff[6] << 8;
        v |= (uint64_t)buff[7];
        return v;
    }
    uint16_t get_short() {
        uint8_t buff[2];
        if (strm->ReadBlock(buff, sizeof(buff)) != sizeof(buff))
            return 0;

        uint16_t v = 0;
        v |= (uint16_t)buff[0] << 8;
        v |= (uint16_t)buff[1];
        return v;
    }
    void send_msg(WebSocketMessage *msg) {
        mtx.Lock();

        uint8_t opcode = msg->isBinary ? 0x2 : 0x1;

        size_t lengthLastByte = msg->data.size() % 4096;
        size_t fullPackets = (msg->data.size() - lengthLastByte) / 4096;
        size_t noPackets = lengthLastByte > 0 ? fullPackets + 1 : fullPackets;
        size_t offset = 0;
        std::vector<uint8_t> mask;
        mask.resize(4);
        for (size_t i = 0; i < noPackets; i++) {
            bool fin = i == noPackets - 1;
            uint8_t finField = fin ? 0b10000000 : 0;
            uint8_t opcode2 = i == 0 ? opcode : 0;
            uint8_t firstByte = finField | (opcode2 & 0xF);

            size_t len = std::min((size_t)4096, msg->data.size() - offset);

            strm->WriteByte(firstByte);
            write_len_bytes((uint64_t)len);

            Crypto::RandomBytes(mask, "Mask it");
            strm->WriteBlock(mask.data(), mask.size());
            for (size_t i = offset; i < offset + len; i++)
                msg->data[i] ^= mask[i % 4];
            strm->WriteBlock(msg->data.data() + offset, len);
            for (size_t i = offset; i < offset + len; i++)
                msg->data[i] ^= mask[i % 4];
            offset += len;
        }
        mtx.Unlock();
    }
    void ping_send(std::vector<uint8_t> &pData) {
        mtx.Lock();

        uint8_t finField = 0b10000000;
        uint8_t firstByte = finField | 0x9;
        strm->WriteByte(firstByte);
        write_len_bytes((uint64_t)pData.size());
        std::vector<uint8_t> mask;
        mask.resize(4);
        Crypto::RandomBytes(mask, "Mask it");
        strm->WriteBlock(mask.data(), mask.size());
        for (size_t i = 0; i < pData.size(); i++)
            pData[i] ^= mask[i % 4];
        strm->WriteBlock(pData.data(), pData.size());
        for (size_t i = 0; i < pData.size(); i++)
            pData[i] ^= mask[i % 4];
        mtx.Unlock();
    }
    void pong_send(std::vector<uint8_t> &pData) {
        mtx.Lock();

        uint8_t finField = 0b10000000;
        uint8_t firstByte = finField | 0xA;
        strm->WriteByte(firstByte);
        write_len_bytes((uint64_t)pData.size());
        strm->WriteBlock(pData.data(), pData.size());
        mtx.Unlock();
    }
    bool read_packet(uint8_t len, std::vector<uint8_t> &data) {

        uint8_t realLen = len & 127;
        bool masked = (len & 0b10000000) > 0;
        uint64_t reallen2 =
            realLen >= 126 ? realLen > 126 ? get_long() : get_short() : realLen;
        uint8_t mask[4];
        if (masked) {
            if (strm->ReadBlock(mask, sizeof(mask)) != sizeof(mask))
                return false;
        }
        size_t offset = data.size();
        data.resize(offset + (size_t)reallen2);
        if (data.size() < ((uint64_t)offset + reallen2))
            return false;
        if (strm->ReadBlock(data.data() + offset, (size_t)reallen2) !=
            (size_t)reallen2)
            return false;
        if (masked) {
            for (size_t i = 0; i < (size_t)reallen2; i++) {
                data[i + offset] ^= mask[i % 4];
            }
        }
        return true;
    }

    WSClient(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,
             std::shared_ptr<WebSocketConnection> conn) {
        this->strm = strm;
        this->conn = conn;
    }
    void Start() {
        this->closed = false;
        bool hasMessage = false;

        WebSocketMessage message;
        message.isBinary = false;
        message.data = {};

        while (!strm->EndOfStream()) {

            uint8_t frame_start[2];
            if (strm->ReadBlock(frame_start, 2) != 2)
                return;

            uint8_t opcode = frame_start[0] & 0xF;
            bool fin = (frame_start[0] & 0b10000000) > 0;
            switch (opcode) {
            case 0x0:
                if (!hasMessage)
                    break;
                read_packet(frame_start[1], message.data);
                break;
            case 0x1:
            case 0x2:
                hasMessage = true;
                message.data = {};
                message.isBinary = opcode == 0x2;

                read_packet(frame_start[1], message.data);
                break;
            case 0x8:
                if (!this->closed)
                    this->close();
                this->conn->OnClose(true);
                return;
            case 0x9: {
                std::vector<uint8_t> b;
                read_packet(frame_start[1], b);
                pong_send(b);
            } break;
            case 0xA: {
                std::vector<uint8_t> b;
                read_packet(frame_start[1], b);
            }
            }
            if (fin && hasMessage) {
                hasMessage = false;
                this->conn->OnReceive(message);
                message.data = {};
            }
        }
        this->conn->OnClose(false);
    }
};

void WebSocketClient(std::string url, HttpDictionary &requestHeaders,
                     std::shared_ptr<WebSocketConnection> wsc,
                     std::function<bool(HttpDictionary &, bool)> cb) {
    WebSocketUnixSocketClient("", url, requestHeaders, wsc, cb);
}

void WebSocketUnixSocketClient(std::string unixSocket, std::string url,
                               HttpDictionary &requestHeaders,
                               std::shared_ptr<WebSocketConnection> wsc,
                               std::function<bool(HttpDictionary &, bool)> cb) {
    HttpRequest req;
    req.url = url;
    req.requestHeaders = requestHeaders;
    req.followRedirects = true;
    req.unixSocket = unixSocket;

    std::string hash = "";

    if (Crypto::HaveCrypto()) {
        std::vector<uint8_t> code;
        code.resize(16);
        Crypto::RandomBytes(code, "Tesses::Framework::Http::WebSocketClient");
        hash = Crypto::Base64_Encode(code);
    }

    req.requestHeaders.SetValue("Sec-WebSocket-Key", hash);
    req.requestHeaders.SetValue("Sec-WebSocket-Version", "13");
    req.requestHeaders.SetValue("Upgrade", "websocket");
    req.requestHeaders.SetValue("Connection", "Upgrade");

    HttpResponse resp(req);

    std::string accept = "0uytbGS5rkQTa6saiHK4AQ==";

    if (resp.statusCode != 101 ||
        !resp.responseHeaders.TryGetFirst("Sec-WebSocket-Accept", accept) ||
        !resp.responseHeaders.AnyEquals("Connection", "Upgrade") ||
        !resp.responseHeaders.AnyEquals("Upgrade", "websocket")) {
        cb(resp.responseHeaders, false);
        return;
    }

    if (Crypto::HaveCrypto()) {
        std::string txt = hash;
        hash += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        if (Crypto::Base64_Encode(Crypto::Sha1::ComputeHash(
                (const uint8_t *)hash.data(), hash.size())) != accept) {
            cb(resp.responseHeaders, false);
            return;
        }
    } else if (accept != "Swjoe56alHg6mvKmKyiDd3tpNqc=") {
        cb(resp.responseHeaders, false);
        return;
    }
    if (!cb(resp.responseHeaders, true)) {

        return;
    }

    WSClient clt(resp.GetInternalStream(), wsc);

    Threading::Thread thrd([&clt, wsc]() -> void {
        try {
            wsc->OnOpen(
                [&clt](WebSocketMessage &msg) -> void { clt.send_msg(&msg); },
                [&clt]() -> void {
                    std::vector<uint8_t> p = {(uint8_t)'p', (uint8_t)'i',
                                              (uint8_t)'n', (uint8_t)'g'};
                    clt.ping_send(p);
                },
                [&clt]() -> void { clt.close(); });
        } catch (...) {
        }
    });

    clt.Start();
    thrd.Join();
}

} // namespace Tesses::Framework::Http
