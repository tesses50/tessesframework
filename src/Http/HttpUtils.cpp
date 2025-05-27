#include "TessesFramework/Http/HttpUtils.hpp"
#include "TessesFramework/Filesystem/VFS.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
using VFSPath = Tesses::Framework::Filesystem::VFSPath;
namespace Tesses::Framework::Http {
    
    bool Uri::Relative(std::string url, Uri& uri)
    {
        auto index = url.find_first_of("//");
        if(index != std::string::npos)
        {
            if(Uri::TryParse(url,uri))
            {
                if(index == 0)
                    uri.scheme = this->scheme;
                return true;
            }
        }
        else if(!url.empty())
        {
            if(url[0] == '/')
            {
                
                auto thirdPart = HttpUtils::SplitString(url,"#",2);
                if(thirdPart.empty()) return false;
                if(thirdPart.size() == 2)
                {
                    uri.hash=thirdPart[1];
                }


                auto fourthPart = HttpUtils::SplitString(thirdPart[1],"?",2);
                
                VFSPath p = fourthPart[0];
                uri.path = p.CollapseRelativeParents().ToString(); //this should be safe
                if(fourthPart.size() == 2)
                {
                    HttpUtils::QueryParamsDecode(uri.query, fourthPart[1]);   
                }
            }
            else
            {
                auto thirdPart = HttpUtils::SplitString(url,"#",2);
                if(thirdPart.empty()) return false;
                if(thirdPart.size() == 2)
                {
                    uri.hash=thirdPart[1];
                }


                auto fourthPart = HttpUtils::SplitString(thirdPart[1],"?",2);
                
                VFSPath p = VFSPath(this->path,fourthPart[0]);
                uri.path = p.CollapseRelativeParents().ToString(); //this should be safe
                if(fourthPart.size() == 2)
                {
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
    std::string Uri::HostPort()
    {
        if(this->port != 0) return this->host + ":" + std::to_string(this->port);
        return this->host;
    }
    uint16_t Uri::GetPort()
    {
        if(this->port != 0) return this->port;

        if(this->scheme == "http:" || this->scheme == "ws:")
            return 80;
        if(this->scheme == "https:" || this->scheme == "wss:")
            return 443;
        if(this->scheme == "sftp:")
            return 22;
        if(this->scheme == "ftp:")
            return 21;
        if(this->scheme == "tftp:")
            return 69;
        return 0;
    }
    bool Uri::TryParse(std::string url, Uri& uri)
    {
        uri.scheme = "";
        uri.port=0;
        auto firstPart = HttpUtils::SplitString(url,"//",2);
        if(firstPart.size() == 2)
       
        uri.scheme=firstPart[0];
        else if(firstPart.empty())
        return false;

            auto secondPart = HttpUtils::SplitString(firstPart.size() == 2 ? firstPart[1] : firstPart[0] ,"/",2);

            if(secondPart.size() == 1)
            {
                uri.path = "/";
            }
            else if(secondPart.size() == 2)
            {
                auto thirdPart = HttpUtils::SplitString(secondPart[1],"#",2);
                if(thirdPart.empty()) return false;
                if(thirdPart.size() == 2)
                {
                    uri.hash=thirdPart[1];
                }


                auto fourthPart = HttpUtils::SplitString(thirdPart[0],"?",2);
                uri.path = "/" + fourthPart[0]; //this should be safe
                if(fourthPart.size() == 2)
                {
                    HttpUtils::QueryParamsDecode(uri.query, fourthPart[1]);   
                }
            }
            else
            {
                return false;
            }
            
            auto hostPortPart = HttpUtils::SplitString(secondPart[0],":",2);

            if(hostPortPart.empty()) return false;
            if(hostPortPart.size() == 2)
            {
                uri.port = (uint16_t)std::stoul(hostPortPart[1]);
            }
            uri.host = hostPortPart[0];
        
        return true;
    }
    std::string Uri::GetPathAndQuery()
    {
        return this->path + this->GetQuery();
    }
    std::string Uri::GetQuery()
    {
        if(this->query.kvp.empty()) return "";
        std::string queryStr = "?";
        queryStr.append(HttpUtils::QueryParamsEncode(query));
        return queryStr;
    }

    std::string Uri::ToString()
    {
        std::string uri = this->scheme;
        uri.append("//");
        uri.append(this->host);
        if(this->port > 0) 
        {
            uri.push_back(':');
            uri.append(std::to_string(this->port));
        }
        uri.append(this->GetPathAndQuery());
        return uri;
    }
    std::string HttpUtils::Replace(std::string text, std::string find, std::string replace)
    {
        std::string dest;
        while(text.length() > 0)
        {
           std::size_t index= text.find(find);

           

           if(index == std::string::npos)
           {
                dest.append(text);
                break;
           }
           else
           {
                std::string left = text.substr(0,index);

                text = text.substr(index+find.size());
                dest.append(left);
                dest.append(replace);

           }
        }
        return dest;
    }
    std::string HttpUtils::LeftPad(std::string text, int count, char c)
    {
        std::stringstream strm(std::ios_base::out);
        strm << std::setfill(c) << std::setw(count) << text;
        return strm.str();
    }
    char HttpUtils::NibbleToHex(uint8_t b)
    {
        b %= 16;
        if(b >= 0 && b <= 9)
            return b + '0';
        if(b >= 10 && b <= 15)
            return b + ('a' - 10);
        return 0;
    }
    uint8_t HttpUtils::HexToNibble(char c)
    {
        if(c >= '0' && c <= '9')
            return (uint8_t)(c - '0');
        
        if(c >= 'A' && c <= 'F')
            return (uint8_t)(c - 55);
        
        if(c >= 'a' && c <= 'f')
            return (uint8_t)(c - 87);

        return 0;
    }

    std::string HttpUtils::MimeType(std::filesystem::path p)
    {
        std::string ext = p.extension().string();
        if(ext == ".html" || ext == ".htm")
        {
            return "text/html";
        }
        if(ext == ".txt" || ext == ".log" || ext == ".twss")
        {
            return "text/plain";
        }
        if(ext == ".woff")
        {
            return "application/x-font-woff";
        }
        if(ext == ".vtt")
        {
            return "text/vtt";
        }
        if(ext == ".svg")
        {
            return "image/svg+xml";
        }
        if(ext == ".webp")
        {
            return "image/webp";
        }
        if(ext == ".vcf")
        {
            return "text/v-card";
        }
        if(ext == ".rss" || ext == ".xml" || ext == ".atom" || ext == ".rdf")
        {
            return "application/xml";
        }
        if(ext == ".js")
        {
            return "text/javascript";
        }
        if(ext == ".json")
        {
            return "application/json";
        }
        if(ext == ".wasm")
        {
            return "application/wasm";
        }
        if(ext == ".png")
        {
            return "image/png";
        }
        if(ext == ".jpg" || ext == ".jpeg")
        {
            return "image/jpeg";
        }
        if(ext == ".css")
        {
            return "text/css";
        }
        if(ext == ".gif")
        {
            return "image/gif";
        }
        if(ext == ".mp4")
        {
            return "video/mp4";
        }
        if(ext == ".mov")
        {
            return "video/quicktime";
        }
        if(ext == ".m4a")
        {
            return "audio/mp4";
        }
        if(ext == ".webm")
        {
            return "video/webm";
        }
        if(ext == ".webmanifest")
        {
            return "application/manifest+json";
        }
        if(ext == ".ico")
        {
            return "image/x-icon";
        }

        return "application/octet-stream";
    }
    bool HttpUtils::Invalid(char c)
    {
        //just do windows because it is the strictist when it comes to windows, mac and linux
        if(c >= 0 && c < 32) return true;
        if(c == 127) return true;
        if(c == '\\') return true;
        if(c == '*') return true;
        if(c == '/') return true;
        if(c == '|') return true;
        if(c == ':') return true;
        if(c == '<') return true;
        if(c == '>') return true;
        if(c == '\"') return true;
        if(c == '?') return true;
        return false;
    }
    std::string HttpUtils::Sanitise(std::string text)
    {
        std::string myStr={};
        for(auto item : text)
        {
            if(Invalid(item)) continue;
            myStr.push_back(item);
        }
        return myStr;
    }

    void HttpUtils::QueryParamsDecode(HttpDictionary& dict,std::string query)
    {
        for(auto item : SplitString(query,"&"))
        {
            std::vector<std::string> ss=SplitString(item,"=",2);
            if(ss.size() >= 1)
            {
                std::string value = {};
                if(ss.size() == 2)
                {
                    value = UrlDecode(ss[1]);
                }
                dict.AddValue(UrlDecode(ss[0]),value);
            }
        }
    }    
    std::string HttpUtils::Join(std::string joinStr, std::vector<std::string> ents)
    {
        std::string str={};
        bool first=true;

        for(auto item : ents)
        {
            if(!first) str.append(joinStr);
            str.append(item);
            first=false;
        }
        return str;
    }
    std::string HttpUtils::QueryParamsEncode(HttpDictionary& dict)
    {
        std::string s={};
        bool first = true;
        for(auto item : dict.kvp)
        {
            for(auto item2 : item.second)
            {
                if(!first)
                {
                    s.push_back('&');
                }
                s.insert(s.size(),UrlEncode(item.first));
                s.push_back('=');
                s.insert(s.size(),UrlEncode(item2));
                first=false;
            }
        }
        return s;
    }

    std::string HttpUtils::UrlDecode(std::string v)
    {   
        std::string s = {};

        for(size_t i = 0;i<v.size();i++)
        {
            if(v[i] == '+')
                s.push_back(' ');
            else if(v[i] == '%')
            {
                i++;
                uint8_t n = HexToNibble(v[i])<<4;
                i++;
                n |= HexToNibble(v[i]);
                s.push_back((char)n);
            }
            else
                s.push_back(v[i]);

        }
        return s;
    }
    std::string HttpUtils::UrlPathEncode(std::string v,bool ignoreSpace)
    {
        std::string s = {};

        for(auto item : v)
        {
            if(item >= 'A' && item <= 'Z')
                s.push_back(item);
            else if(item >= 'a' && item <= 'z')
                s.push_back(item);
            else if(item >= '0' && item <= '9')
                s.push_back(item);
            else if(item == '-' || item == '_' || item == '.' || item == '~' || item == '/')
                s.push_back(item);
            else
            {
                if(item != ' ' || !ignoreSpace)
                {
                    s.push_back('%');
                    s.push_back(NibbleToHex((item >> 4) & 0xF));
                    s.push_back(NibbleToHex((item) & 0xF));
                }
                else
                {
                    s.push_back(' ');
                }
            }
        }
        return s;
    }
    std::string HttpUtils::UrlPathDecode(std::string v)
    {
        std::string s = {};

        for(size_t i = 0;i<v.size();i++)
        {
            if(v[i] == '%')
            {
                i++;
                uint8_t n = HexToNibble(v[i])<<4;
                i++;
                n |= HexToNibble(v[i]);
                s.push_back((char)n);
            }
            else
                s.push_back(v[i]);

        }
        return s;
    }

    std::string HttpUtils::UrlEncode(std::string v)
    {
        std::string s = {};

        for(auto item : v)
        {
            if(item == ' ')
                s.push_back('+');
            else if(item >= 'A' && item <= 'Z')
                s.push_back(item);
            else if(item >= 'a' && item <= 'z')
                s.push_back(item);
            else if(item >= '0' && item <= '9')
                s.push_back(item);
            else if(item == '-' || item == '_' || item == '.' || item == '~')
                s.push_back(item);
            else
            {
                s.push_back('%');
                s.push_back(NibbleToHex((item >> 4) & 0xF));
                s.push_back(NibbleToHex((item) & 0xF));
            }
        }
        return s;
    }

    std::vector<std::string> HttpUtils::SplitString(std::string text, std::string delimiter,std::size_t maxCnt)
    {
        std::vector<std::string> strs;
        std::size_t i = 1;
        while(text.length() > 0)
        {
            if(i == maxCnt)
           {
                strs.push_back(text);
                break;    
           }
           std::size_t index= text.find(delimiter);

           

           if(index == std::string::npos)
           {
                strs.push_back(text);
                break;
           }
           else
           {
                std::string left = text.substr(0,index);

                text = text.substr(index+delimiter.size());

                strs.push_back(left);

           }
           i++;
        }
        return strs;
    }
    std::string HttpUtils::HtmlEncode(std::string html)
    {
        std::string myHtml = {};
        for(auto item : html)
        {
            if(item == '\"')
            {
                myHtml.append("&quot;");
            }
            else if(item == '\'')
            {
                myHtml.append("&apos;");
            }
            else if(item == '&')
            {
                myHtml.append("&amp;");
            }
            else if(item == '<')
            {
                myHtml.append("&lt;");
            }
            else if(item == '>')
            {
                myHtml.append("&gt;");
            }
            else
            {
                myHtml.push_back(item);
            }
        }
        return myHtml;
    }
    std::string HttpUtils::StatusCodeString(StatusCode code)
    {
        switch(code)
        {
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
    
    bool HttpDictionary::AnyEquals(std::string key, std::string value)
    {
        if(this->kvp.count(key) > 0)
            for(auto v : this->kvp[key]) if(v == value) return true;
        // for(auto v : item.second) if(v == value) return true;
        return false;
    }

    void HttpDictionary::Clear()
    {
        kvp.clear();
    }
    void HttpDictionary::Clear(std::string key, bool kvpExistsAfter)
    {
        if(kvpExistsAfter)
        {
            kvp[key].clear();
        }
        else
        {
            if(kvp.count(key) == 0) return;
            kvp[key].clear();
            kvp.erase(key);
        }
    }
    void HttpDictionary::SetValue(std::string key, std::string value)
    {
        kvp[key] = {value};
    }
    void HttpDictionary::SetValue(std::string key, int64_t value)
    {
        kvp[key] = {std::to_string(value)};
    }
    void HttpDictionary::SetValue(std::string key, double value)
    {
        kvp[key] = {std::to_string(value)};
    }
    void HttpDictionary::SetValue(std::string key, Date::DateTime value)
    {
        kvp[key] = {value.ToHttpDate()};
    }
    void HttpDictionary::SetValue(std::string key, std::vector<std::string> value)
    {
        kvp[key] = value;
    }
    void HttpDictionary::AddValue(std::string key, std::string value)
    {
        kvp[key].push_back(value);
    }
    void HttpDictionary::AddValue(std::string key, int64_t value)
    {
        kvp[key].push_back(std::to_string(value));
    }

    void HttpDictionary::AddValue(std::string key, double value)
    {
        kvp[key].push_back(std::to_string(value));
    }

    void HttpDictionary::AddValue(std::string key, Date::DateTime value)
    {
        kvp[key].push_back(value.ToHttpDate());
    }
    void HttpDictionary::AddValue(std::string key, std::vector<std::string> value)
    {
        auto& ls = kvp[key];
        ls.insert(ls.end(), value.begin(), value.end());
    }
    bool HttpDictionary::TryGetFirst(std::string key, std::string& value)
    {
        if(kvp.count(key) == 0) return false;
        auto& ls = kvp[key];
        if(ls.empty()) return false;
        value = ls.front();

        return true;
    }

    bool HttpDictionary::TryGetFirstInt(std::string key, int64_t& value)
    {
        std::string val;
        if(!TryGetFirst(key,val)) return false;
        try{
            size_t off = 0;
            auto v = std::stoll(val,&off);
            if(off != val.size()) return false;
            value = v;
        } 
        catch(std::exception& ex)
        {
            return false;
        }
        return true;
    }
    bool HttpDictionary::TryGetFirstDate(std::string key, Date::DateTime& dt)
    {
        std::string val;
        if(!TryGetFirst(key,val)) return false;
        return Date::DateTime::TryParseHttpDate(val,dt);
    }
    bool HttpDictionary::TryGetFirstDouble(std::string key, double& value)
    {
        std::string val;
        if(!TryGetFirst(key,val)) return false;
        try{
            size_t off = 0;
            auto v = std::stod(val,&off);
            if(off != val.size()) return false;
            value = v;
        } 
        catch(std::exception& ex)
        {
            return false;
        }
        return true;
    }
     bool CaseInsensitiveLess::operator() (const std::string& s1, const std::string& s2) const {
        return  HttpUtils::ToLower(s1) < HttpUtils::ToLower(s2);
        
    }

    std::string HttpUtils::ToLower(std::string str)
    {
        std::string str1(str.length(),' ');
        std::transform(str.begin(), str.end(), str1.begin(), tolower);
        return str1;
    }

    std::string HttpUtils::ToUpper(std::string str)
    {
        std::string str1(str.length(),' ');
        std::transform(str.begin(), str.end(), str1.begin(), toupper);
        return str1;
    }

    bool HttpDictionary::GetFirstBoolean(std::string key)
    {
        std::string val;
        if(!TryGetFirst(key,val)) return false;
        return val == "true" || val == "on";
    }
    
}
