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

#include "TessesFramework/Http/HttpUtils.hpp"
#include "TessesFramework/Filesystem/VFS.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
using VFSPath = Tesses::Framework::Filesystem::VFSPath;
namespace Tesses::Framework::Http {

bool Uri::Relative(std::string url, Uri &uri) {
    auto index = url.find_first_of("//");
    if (index != std::string::npos) {
        if (Uri::TryParse(url, uri)) {
            if (index == 0)
                uri.scheme = this->scheme;
            return true;
        }
    } else if (!url.empty()) {
        if (url[0] == '/') {

            auto thirdPart = HttpUtils::SplitString(url, "#", 2);
            if (thirdPart.empty())
                return false;
            if (thirdPart.size() == 2) {
                uri.hash = thirdPart[1];
            }

            auto fourthPart = HttpUtils::SplitString(thirdPart[1], "?", 2);

            VFSPath p = fourthPart[0];
            uri.path =
                p.CollapseRelativeParents().ToString(); // this should be safe
            if (fourthPart.size() == 2) {
                HttpUtils::QueryParamsDecode(uri.query, fourthPart[1]);
            }
        } else {
            auto thirdPart = HttpUtils::SplitString(url, "#", 2);
            if (thirdPart.empty())
                return false;
            if (thirdPart.size() == 2) {
                uri.hash = thirdPart[1];
            }

            auto fourthPart = HttpUtils::SplitString(thirdPart[1], "?", 2);

            VFSPath p = VFSPath(this->path, fourthPart[0]);
            uri.path =
                p.CollapseRelativeParents().ToString(); // this should be safe
            if (fourthPart.size() == 2) {
                HttpUtils::QueryParamsDecode(uri.query, fourthPart[1]);
            }
        }
        uri.scheme = this->scheme;
        uri.host = this->host;
        uri.port = this->port;
        return true;
    }
    return false;
}
std::string Uri::HostPort() {
    if (this->port != 0)
        return this->host + ":" + std::to_string(this->port);
    return this->host;
}
uint16_t Uri::GetPort() {
    if (this->port != 0)
        return this->port;

    if (this->scheme == "http:" || this->scheme == "ws:")
        return 80;
    if (this->scheme == "https:" || this->scheme == "wss:")
        return 443;
    if (this->scheme == "sftp:")
        return 22;
    if (this->scheme == "ftp:")
        return 21;
    if (this->scheme == "tftp:")
        return 69;
    return 0;
}
bool Uri::TryParse(std::string url, Uri &uri) {
    uri.scheme = "";
    uri.port = 0;
    auto firstPart = HttpUtils::SplitString(url, "//", 2);
    if (firstPart.size() == 2)

        uri.scheme = firstPart[0];
    else if (firstPart.empty())
        return false;

    auto secondPart = HttpUtils::SplitString(
        firstPart.size() == 2 ? firstPart[1] : firstPart[0], "/", 2);

    if (secondPart.size() == 1) {
        uri.path = "/";
    } else if (secondPart.size() == 2) {
        auto thirdPart = HttpUtils::SplitString(secondPart[1], "#", 2);
        if (thirdPart.empty())
            return false;
        if (thirdPart.size() == 2) {
            uri.hash = thirdPart[1];
        }

        auto fourthPart = HttpUtils::SplitString(thirdPart[0], "?", 2);
        uri.path = "/" + fourthPart[0]; // this should be safe
        if (fourthPart.size() == 2) {
            HttpUtils::QueryParamsDecode(uri.query, fourthPart[1]);
        }
    } else {
        return false;
    }

    auto hostPortPart = HttpUtils::SplitString(secondPart[0], ":", 2);

    if (hostPortPart.empty())
        return false;
    if (hostPortPart.size() == 2) {
        uri.port = (uint16_t)std::stoul(hostPortPart[1]);
    }
    uri.host = hostPortPart[0];

    return true;
}
Uri::Uri() : query(true) {}
std::string Uri::GetPathAndQuery() { return this->path + this->GetQuery(); }
std::string Uri::GetQuery() {
    if (this->query.kvp.empty())
        return "";
    std::string queryStr = "?";
    queryStr.append(HttpUtils::QueryParamsEncode(query));
    return queryStr;
}

std::string Uri::ToString() {
    std::string uri = this->scheme;
    uri.append("//");
    uri.append(this->host);
    if (this->port > 0) {
        uri.push_back(':');
        uri.append(std::to_string(this->port));
    }
    uri.append(this->GetPathAndQuery());
    return uri;
}
std::string HttpUtils::Replace(std::string_view text, std::string_view find,
                               std::string_view replace) {
    if (find.empty())
        return std::string(text);
    std::string dest;
    while (text.length() > 0) {
        std::size_t index = text.find(find);

        if (index == std::string_view::npos) {
            dest.append(text);
            break;
        } else {
            std::string_view left = text.substr(0, index);

            text = text.substr(index + find.size());
            dest.append(left);
            dest.append(replace);
        }
    }
    return dest;
}

std::string HttpUtils::LeftPad(std::string_view text, int count, char c) {
    if (text.size() >= count)
        return std::string(text);
    std::string newStr(count - text.size(), c);
    newStr.append(text);
    return newStr;
}
char HttpUtils::NibbleToHex(uint8_t b, bool isUppercase) {
    if (isUppercase) {
        b %= 16;
        if (b >= 0 && b <= 9)
            return b + '0';
        if (b >= 10 && b <= 15)
            return b + ('A' - 10);
        return 0;
    }
    return NibbleToHex(b);
}
char HttpUtils::NibbleToHex(uint8_t b) {
    b %= 16;
    if (b >= 0 && b <= 9)
        return b + '0';
    if (b >= 10 && b <= 15)
        return b + ('a' - 10);
    return 0;
}
uint8_t HttpUtils::HexToNibble(char c) {
    if (c >= '0' && c <= '9')
        return (uint8_t)(c - '0');

    if (c >= 'A' && c <= 'F')
        return (uint8_t)(c - 55);

    if (c >= 'a' && c <= 'f')
        return (uint8_t)(c - 87);

    return 0;
}

struct mimetype_reg {
    Threading::Mutex mtx;
    std::unordered_map<std::string, std::string> map = {
        {".html", "text/html"},
        {".htm", "text/html"},
        {".txt", "text/plain"},
        {".log", "text/plain"},
        {".twss", "text/twss"},
        {".css", "text/css"},
        {".js", "text/javascript"},
        {".mjs", "text/javascript"},
        {".json", "application/json"},
        {".map", "application/json"},
        {".pdf", "application/pdf"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".webp", "image/webp"},
        {".svg", "image/svg+xml"},
        {".ico", "image/x-icon"},
        {".bmp", "image/bmp"},
        {".avif", "image/avif"},
        {".heic", "image/heic"},
        {".tif", "image/tiff"},
        {".tiff", "image/tiff"},
        {".mp4", "video/mp4"},
        {".webm", "video/webm"},
        {".mov", "video/quicktime"},
        {".mkv", "video/x-matroska"},
        {".avi", "video/x-msvideo"},
        {".ts", "video/mp2t"},
        {".mp3", "audio/mpeg"},
        {".m4a", "audio/mp4"},
        {".ogg", "audio/ogg"},
        {".oga", "audio/ogg"},
        {".opus", "audio/ogg"},
        {".wav", "audio/wav"},
        {".flac", "audio/flac"},
        {".aac", "audio/aac"},
        {".weba", "audio/webm"},
        {".woff", "font/woff"},
        {".woff2", "font/woff2"},
        {".ttf", "font/ttf"},
        {".zip", "application/zip"},
        {".gz", "application/gzip"},
        {".7z", "application/x-7z-compressed"},
        {".rar", "application/vnd.rar"},
        {".csv", "text/csv"},
        {".vtt", "text/vtt"},
        {".vcf", "text/v-card"},
        {".rss", "application/xml"},
        {".xml", "application/xml"},
        {".atom", "application/xml"},
        {".rdf", "application/xml"},
        {".wasm", "application/wasm"},
        {".webmanifest", "application/manifest+json"},
        {".m3u8", "application/vnd.apple.mpegurl"},
        {".mpd", "application/dash+xml"},
        {".md", "text/markdown"},
        {".epub", "application/epub+zip"},
        {".ics", "text/calendar"},
        {".apk", "application/vnd.android.package-archive"},
        {".crvm", "application/crvm"},
        {".tcross", "text/tcross"},
    };
    std::string Get(const std::string &ext) {
        Threading::LockGuard lg(mtx);
        std::string mime;

        if (map.count(ext) != 0)
            mime = map[ext];

        if (mime.empty())
            return "application/octet-stream";
        return mime;
    }
    void Add(const std::string &ext, const std::string &mime) {
        Threading::LockGuard lg(mtx);
        map[ext] = mime;
    }
};

static mimetype_reg mime_reg;

std::string HttpUtils::GetMimeType(const std::string &ext) {
    return mime_reg.Get(ext);
}
std::string HttpUtils::GetMimeTypePath(const Filesystem::VFSPath &p) {
    return GetMimeType(p.GetExtension());
}
void HttpUtils::AddMimeType(const std::string &ext, const std::string &mime) {
    mime_reg.Add(ext, mime);
}
void HttpUtils::AddMimeTypePath(const Filesystem::VFSPath &p,
                                const std::string &mime) {
    AddMimeType(p.GetExtension(), mime);
}

bool HttpUtils::Invalid(char c) {
    // just do windows because it is the strictist when it comes to windows, mac
    // and linux
    if (c >= 0 && c < 32)
        return true;
    if (c == 127)
        return true;
    if (c == '\\')
        return true;
    if (c == '*')
        return true;
    if (c == '/')
        return true;
    if (c == '|')
        return true;
    if (c == ':')
        return true;
    if (c == '<')
        return true;
    if (c == '>')
        return true;
    if (c == '\"')
        return true;
    if (c == '?')
        return true;
    return false;
}
std::string HttpUtils::Sanitise(std::string_view text) {
    std::string myStr = {};
    for (auto item : text) {
        if (Invalid(item))
            continue;
        myStr.push_back(item);
    }
    return myStr;
}

void HttpUtils::QueryParamsDecode(HttpDictionary &dict,
                                  std::string_view query) {
    for (auto item : SplitString(query, "&")) {
        std::vector<std::string> ss = SplitString(item, "=", 2);
        if (ss.size() >= 1) {
            std::string value = {};
            if (ss.size() == 2) {
                value = UrlDecode(ss[1]);
            }
            dict.AddValue(UrlDecode(ss[0]), value);
        }
    }
}
std::string HttpUtils::Join(std::string_view joinStr,
                            std::vector<std::string> ents) {
    std::string str = {};
    bool first = true;

    for (auto item : ents) {
        if (!first)
            str.append(joinStr);
        str.append(item);
        first = false;
    }
    return str;
}
std::string HttpUtils::QueryParamsEncode(HttpDictionary &dict) {
    std::string s = {};
    bool first = true;
    for (auto item : dict.kvp) {
        for (auto item2 : item.second) {
            if (!first) {
                s.push_back('&');
            }
            s.insert(s.size(), UrlEncode(item.first));
            s.push_back('=');
            s.insert(s.size(), UrlEncode(item2));
            first = false;
        }
    }
    return s;
}

std::string HttpUtils::UrlDecode(std::string_view v) {
    std::string s = {};

    for (size_t i = 0; i < v.size(); i++) {
        if (v[i] == '+')
            s.push_back(' ');
        else if (v[i] == '%') {
            if (i + 2 >= v.size())
                break;

            uint8_t n = (HexToNibble(v[i + 1]) << 4) | (HexToNibble(v[i + 2]));
            i += 2;
            s.push_back(static_cast<char>(n));
        } else
            s.push_back(v[i]);
    }
    return s;
}
std::string HttpUtils::UrlPathEncode(std::string_view v, bool ignoreSpace) {
    std::string s = {};

    for (auto item : v) {
        uint8_t byte = static_cast<uint8_t>(item);
        if (byte >= 'A' && byte <= 'Z')
            s.push_back(item);
        else if (byte >= 'a' && byte <= 'z')
            s.push_back(item);
        else if (byte >= '0' && byte <= '9')
            s.push_back(item);
        else if (byte == '-' || byte == '_' || byte == '.' || byte == '~' ||
                 byte == '/')
            s.push_back(item);
        else {
            if (byte != ' ' || !ignoreSpace) {
                s.push_back('%');
                s.push_back(NibbleToHex((byte >> 4) & 0xF));
                s.push_back(NibbleToHex((byte) & 0xF));
            } else {
                s.push_back(' ');
            }
        }
    }
    return s;
}
std::string HttpUtils::UrlPathDecode(std::string_view v) {
    std::string s = {};

    for (size_t i = 0; i < v.size(); i++) {
        if (v[i] == '%') {
            if (i + 2 >= v.size())
                break;

            uint8_t n = (HexToNibble(v[i + 1]) << 4) | (HexToNibble(v[i + 2]));
            i += 2;
            s.push_back(static_cast<char>(n));
        } else
            s.push_back(v[i]);
    }
    return s;
}

std::string HttpUtils::UrlEncode(std::string_view v) {
    std::string s = {};

    for (auto item : v) {
        uint8_t byte = static_cast<uint8_t>(item);
        if (byte == ' ')
            s.push_back('+');
        else if (byte >= 'A' && byte <= 'Z')
            s.push_back(item);
        else if (byte >= 'a' && byte <= 'z')
            s.push_back(item);
        else if (byte >= '0' && byte <= '9')
            s.push_back(item);
        else if (byte == '-' || byte == '_' || byte == '.' || byte == '~')
            s.push_back(item);
        else {
            s.push_back('%');
            s.push_back(NibbleToHex((byte >> 4) & 0xF));
            s.push_back(NibbleToHex((byte) & 0xF));
        }
    }
    return s;
}
void HttpUtils::SplitString(std::vector<std::string> &strs,
                            std::string_view text, std::string_view delimiter,
                            std::size_t maxCnt) {
    if (maxCnt == 0)
        return;
    if (delimiter.empty()) {
        strs.push_back(std::string(text));
        return;
    }
    std::size_t i = 1;
    while (true) {
        if (i == maxCnt) {
            strs.push_back(std::string(text));
            break;
        }
        std::size_t index = text.find(delimiter);

        if (index == std::string_view::npos) {
            strs.push_back(std::string(text));
            break;
        }
        std::string_view left = text.substr(0, index);

        text = text.substr(index + delimiter.size());

        strs.push_back(std::string(left));

        i++;
    }
}

std::vector<std::string> HttpUtils::SplitString(std::string_view text,
                                                std::string_view delimiter,
                                                std::size_t maxCnt) {
    std::vector<std::string> strs;
    SplitString(strs, text, delimiter, maxCnt);

    return strs;
}

std::string HttpUtils::HtmlP(std::string_view text) {
    std::string newText = "";
    std::string builder = "";

    auto flush = [&]() -> void {
        if (!builder.empty()) {
            if (builder.find("http://") == 0 || builder.find("https://") == 0 ||
                builder.find("ftp://") == 0 || builder.find("ftps://") == 0 ||
                builder.find("magnet:") == 0 || builder.find("btmh:") == 0) {
                newText += "<a href=\"" + HttpUtils::HtmlEncode(builder) +
                           "\">" + HttpUtils::HtmlEncode(builder) + "</a>";
            } else if (builder.find("mailto:") == 0) {
                newText += "<a href=\"" + HttpUtils::HtmlEncode(builder) +
                           "\">" + HttpUtils::HtmlEncode(builder.substr(7)) +
                           "</a>";
            } else if (builder.find("tel:") == 0) {
                newText += "<a href=\"" + HttpUtils::HtmlEncode(builder) +
                           "\">" + HttpUtils::HtmlEncode(builder.substr(4)) +
                           "</a>";
            } else {
                newText += HttpUtils::HtmlEncode(builder);
            }
            builder = "";
        }
    };

    for (auto item : text) {
        switch (item) {
        case ' ':
            flush();
            newText += " ";
            break;
        case '\n':
            flush();
            newText += "<br>";
            break;
        case '\t':
            flush();
            newText += "&tab;";
            break;
        case '\r':
            flush();
            break;
        default:
            builder += item;
            break;
        }
    }
    flush();

    return newText;
}
std::string HttpUtils::HtmlEncode(std::string_view html) {
    std::string myHtml = {};
    for (auto item : html) {
        if (item == '\"') {
            myHtml.append("&quot;");
        } else if (item == '\'') {
            myHtml.append("&apos;");
        } else if (item == '&') {
            myHtml.append("&amp;");
        } else if (item == '<') {
            myHtml.append("&lt;");
        } else if (item == '>') {
            myHtml.append("&gt;");
        } else {
            myHtml.push_back(item);
        }
    }
    return myHtml;
}
std::string HttpUtils::StatusCodeString(StatusCode code) {
    switch (code) {
    case StatusCode::Continue:
        return "Continue";
    case StatusCode::SwitchingProtocols:
        return "Switching Protocols";
    case StatusCode::Processing:
        return "Processing";
    case StatusCode::EarlyHints:
        return "Early Hints";
    case StatusCode::OK:
        return "OK";
    case StatusCode::Created:
        return "Created";
    case StatusCode::Accepted:
        return "Accepted";
    case StatusCode::NonAuthoritativeInformation:
        return "Non-Authoritative Information";
    case StatusCode::NoContent:
        return "No Content";
    case StatusCode::ResetContent:
        return "Reset Content";
    case StatusCode::PartialContent:
        return "PartialContent";
    case StatusCode::MultiStatus:
        return "Multi-Status";
    case StatusCode::AlreadyReported:
        return "Already Reported";
    case StatusCode::IMUsed:
        return "IM Used";
    case StatusCode::MultipleChoices:
        return "Multiple Choices";
    case StatusCode::MovedPermanently:
        return "Moved Permanently";
    case StatusCode::Found:
        return "Found";
    case StatusCode::SeeOther:
        return "See Other";
    case StatusCode::NotModified:
        return "Not Modified";
    case StatusCode::UseProxy:
        return "Use Proxy";
    case StatusCode::TemporaryRedirect:
        return "Temporary Redirect";
    case StatusCode::PermanentRedirect:
        return "Permanent Redirect";
    case StatusCode::BadRequest:
        return "Bad Request";
    case StatusCode::Unauthorized:
        return "Unauthorized";
    case StatusCode::PaymentRequired:
        return "Payment Required";
    case StatusCode::Forbidden:
        return "Forbidden";
    case StatusCode::NotFound:
        return "Not Found";
    case StatusCode::MethodNotAllowed:
        return "Method Not Allowed";
    case StatusCode::NotAcceptable:
        return "Not Acceptable";
    case StatusCode::ProxyAuthenticationRequired:
        return "Proxy Authentication Required";
    case StatusCode::RequestTimeout:
        return "Request Timeout";
    case StatusCode::Conflict:
        return "Conflict";
    case StatusCode::Gone:
        return "Gone";
    case StatusCode::LengthRequired:
        return "Length Required";
    case StatusCode::PreconditionFailed:
        return "Precondition Failed";
    case StatusCode::PayloadTooLarge:
        return "Payload Too Large";
    case StatusCode::URITooLong:
        return "URI Too Long";
    case StatusCode::UnsupportedMediaType:
        return "Unsupported Media Type";
    case StatusCode::RangeNotSatisfiable:
        return "Range Not Satisfiable";
    case StatusCode::ExpectationFailed:
        return "Expectation Failed";
    case StatusCode::ImATeapot:
        return "I'm a teapot";
    case StatusCode::MisdirectedRequest:
        return "Misdirected Request";
    case StatusCode::UnprocessableContent:
        return "Unprocessable Content";
    case StatusCode::Locked:
        return "Locked";
    case StatusCode::FailedDependency:
        return "Failed Dependency";
    case StatusCode::TooEarly:
        return "Too Early";
    case StatusCode::UpgradeRequired:
        return "Upgrade Required";
    case StatusCode::PreconditionRequired:
        return "Precondition Required";
    case StatusCode::TooManyRequests:
        return "Too Many Requests";
    case StatusCode::RequestHeaderFieldsTooLarge:
        return "Request Header Fields Too Large";
    case StatusCode::UnavailableForLegalReasons:
        return "Unavailable For Legal Reasons";
    case StatusCode::InternalServerError:
        return "Internal Server Error";
    case StatusCode::NotImplemented:
        return "Not Implemented";
    case StatusCode::ServiceUnavailable:
        return "Service Unavailable";
    case StatusCode::GatewayTimeout:
        return "Gateway Timeout";
    case StatusCode::HTTPVersionNotSupported:
        return "HTTP Version Not Supported";
    case StatusCode::VariantAlsoNegotiates:
        return "Variant Also Negotiates";
    case StatusCode::InsufficientStorage:
        return "Insufficient Storage";
    case StatusCode::LoopDetected:
        return "Loop Detected";
    case StatusCode::NotExtended:
        return "Not Extended";
    case StatusCode::NetworkAuthenticationRequired:
        return "Network Authentication Required";
    default:
        return "";
    }
}

CaseInsensitiveLess::CaseInsensitiveLess(bool caseSensitive) {
    this->caseSensitive = caseSensitive;
}
HttpDictionary::HttpDictionary(bool isCaseSensitive)
    : kvp(CaseInsensitiveLess(isCaseSensitive)) {}
bool HttpDictionary::AnyEquals(std::string key, std::string value) {
    if (this->kvp.count(key) > 0)
        for (auto v : this->kvp[key])
            if (v == value)
                return true;
    // for(auto v : item.second) if(v == value) return true;
    return false;
}
bool HttpUtils::CaseInsensitiveCompare(std::string_view left,
                                       std::string_view right) {
    if (left.size() != right.size())
        return false;

    for (size_t i = 0; i < left.size(); i++) {
        unsigned char senleft = static_cast<unsigned char>(left[i]);
        unsigned char senright = static_cast<unsigned char>(right[i]);
        unsigned char insenleft = (senleft & 0b11011111);
        unsigned char insenright = (senright & 0b11011111);

        if (insenleft >= 'A' && insenleft <= 'Z' && insenright >= 'A' &&
            insenright <= 'Z') {
            if (insenleft != insenright)
                return false;
        } else if (senleft != senright)
            return false;
    }
    return true;
}
bool HttpDictionary::AnyEqualsCSV(std::string key, std::string value) {

    if (this->kvp.count(key)) {
        for (std::string v : this->kvp[key]) {
            auto items = HttpUtils::SplitString(v, ",");
            for (auto item : items) {
                std::string_view itm = item;
                size_t start = itm.find_first_not_of(" \t");
                if (start == std::string::npos)
                    continue; // Empty
                size_t end = itm.find_last_not_of(" \t");
                std::string_view val = itm.substr(start, end - start + 1);
                if (HttpUtils::CaseInsensitiveCompare(value, val))
                    return true;
            }
        }
    }
    return false;
}

void HttpDictionary::Clear() { kvp.clear(); }
void HttpDictionary::Clear(std::string key, bool kvpExistsAfter) {
    if (kvpExistsAfter) {
        kvp[key].clear();
    } else {
        if (kvp.count(key) == 0)
            return;
        kvp[key].clear();
        kvp.erase(key);
    }
}
void HttpDictionary::SetValue(std::string key, std::string value) {
    kvp[key] = {value};
}
void HttpDictionary::SetValue(std::string key, int64_t value) {
    kvp[key] = {std::to_string(value)};
}
void HttpDictionary::SetValue(std::string key, double value) {
    kvp[key] = {std::to_string(value)};
}
void HttpDictionary::SetValue(std::string key, Date::DateTime value) {
    kvp[key] = {value.ToHttpDate()};
}
void HttpDictionary::SetValue(std::string key, std::vector<std::string> value) {
    kvp[key] = value;
}
void HttpDictionary::AddValue(std::string key, std::string value) {
    kvp[key].push_back(value);
}
void HttpDictionary::AddValue(std::string key, int64_t value) {
    kvp[key].push_back(std::to_string(value));
}

void HttpDictionary::AddValue(std::string key, double value) {
    kvp[key].push_back(std::to_string(value));
}

void HttpDictionary::AddValue(std::string key, Date::DateTime value) {
    kvp[key].push_back(value.ToHttpDate());
}
void HttpDictionary::AddValue(std::string key, std::vector<std::string> value) {
    auto &ls = kvp[key];
    ls.insert(ls.end(), value.begin(), value.end());
}
bool HttpDictionary::TryGetFirst(std::string key, std::string &value) {
    if (kvp.count(key) == 0)
        return false;
    auto &ls = kvp[key];
    if (ls.empty())
        return false;
    value = ls.front();

    return true;
}

bool HttpDictionary::TryGetFirstInt(std::string key, int64_t &value) {
    std::string val;
    if (!TryGetFirst(key, val))
        return false;
    try {
        size_t off = 0;
        auto v = std::stoll(val, &off);
        if (off != val.size())
            return false;
        value = v;
    } catch (std::exception &ex) {
        return false;
    }
    return true;
}
bool HttpDictionary::TryGetFirstDate(std::string key, Date::DateTime &dt) {
    std::string val;
    if (!TryGetFirst(key, val))
        return false;
    return Date::DateTime::TryParseHttpDate(val, dt);
}
bool HttpDictionary::TryGetFirstDouble(std::string key, double &value) {
    std::string val;
    if (!TryGetFirst(key, val))
        return false;
    try {
        size_t off = 0;
        auto v = std::stod(val, &off);
        if (off != val.size())
            return false;
        value = v;
    } catch (std::exception &ex) {
        return false;
    }
    return true;
}

bool HttpDictionary::TryGetOnlyOne(std::string key, std::string &value) {
    if (kvp.count(key) == 0)
        return false;
    auto &ls = kvp[key];
    if (ls.size() != 1)
        return false;
    value = ls.front();

    return true;
}

bool HttpDictionary::TryGetOnlyOneInt(std::string key, int64_t &value) {
    std::string val;
    if (!TryGetOnlyOne(key, val))
        return false;
    try {
        size_t off = 0;
        auto v = std::stoll(val, &off);
        if (off != val.size())
            return false;
        value = v;
    } catch (std::exception &ex) {
        return false;
    }
    return true;
}

bool HttpDictionary::TryGetOnlyOneDouble(std::string key, double &value) {
    std::string val;
    if (!TryGetOnlyOne(key, val))
        return false;
    try {
        size_t off = 0;
        auto v = std::stod(val, &off);
        if (off != val.size())
            return false;
        value = v;
    } catch (std::exception &ex) {
        return false;
    }
    return true;
}
bool HttpDictionary::TryGetOnlyOneDate(std::string key, Date::DateTime &value) {
    std::string val;
    if (!TryGetOnlyOne(key, val))
        return false;
    return Date::DateTime::TryParseHttpDate(val, value);
}

bool HttpDictionary::TryGetOnlyOneBoolean(std::string key, bool &val) {
    val = false;
    if (kvp.count(key) == 0)
        return true;

    auto &ls = kvp[key];
    if (ls.size() != 1)
        return false;

    auto front = ls.front();
    val = front == "on" || front == "true";

    return true;
}

bool CaseInsensitiveLess::operator()(const std::string &s1,
                                     const std::string &s2) const {
    if (this->caseSensitive)
        return s1 < s2;
    return std::lexicographical_compare(
        s1.begin(), s1.end(), s2.begin(), s2.end(), [](char a, char b) {
            return std::tolower(static_cast<unsigned char>(a)) <
                   std::tolower(static_cast<unsigned char>(b));
        });
}

std::string HttpUtils::ToLower(std::string_view str) {
    std::string str1(str.length(), ' ');
    std::transform(str.begin(), str.end(), str1.begin(), tolower);
    return str1;
}

std::string HttpUtils::ToUpper(std::string_view str) {
    std::string str1(str.length(), ' ');
    std::transform(str.begin(), str.end(), str1.begin(), toupper);
    return str1;
}

bool HttpDictionary::GetFirstBoolean(std::string key) {
    std::string val;
    if (!TryGetFirst(key, val))
        return false;
    return val == "true" || val == "on";
}
std::string HttpUtils::BytesToHex(const std::vector<uint8_t> &data) {
    std::string text;
    BytesToHex(text, data);
    return text;
}
std::string HttpUtils::BytesToHex(const std::vector<uint8_t> &data,
                                  bool isUpper) {
    std::string text;
    BytesToHex(text, data, isUpper);
    return text;
}
void HttpUtils::BytesToHex(std::string &text,
                           const std::vector<uint8_t> &data) {
    if (data.empty()) {
        text.clear();
        return;
    }
    text.resize(data.size() * 2);
    for (size_t i = 0; i < data.size(); i++) {
        text[i * 2] = NibbleToHex(data[i] >> 4);
        text[i * 2 + 1] += NibbleToHex(data[i]);
    }
}

void HttpUtils::BytesToHex(std::string &text, const std::vector<uint8_t> &data,
                           bool isUpper) {
    if (data.empty()) {
        text.clear();
        return;
    }
    text.resize(data.size() * 2);
    for (size_t i = 0; i < data.size(); i++) {
        text[i * 2] = NibbleToHex(data[i] >> 4, isUpper);
        text[i * 2 + 1] += NibbleToHex(data[i], isUpper);
    }
}
std::vector<uint8_t> HttpUtils::HexToBytes(std::string_view text) {
    std::vector<uint8_t> data;
    HexToBytes(data, text);
    return data;
}
void HttpUtils::HexToBytes(std::vector<uint8_t> &data, std::string_view text) {
    if (text.empty()) {
        data.clear();
        return;
    }
    if (text.size() % 2 != 0)
        throw std::runtime_error("Text length is not even");

    data.resize(text.size() / 2);
    for (size_t i = 0; i < text.size(); i += 2) {
        uint8_t b = HexToNibble(text[i]) << 4;
        b |= HexToNibble(text[i + 1]);
        data[i / 2] = b;
    }
}
} // namespace Tesses::Framework::Http
