#include "TessesFramework/Http/HttpClient.hpp"
#include "TessesFramework/Crypto/ClientTLSStream.hpp"
#include "TessesFramework/Streams/NetworkStream.hpp"
#include "TessesFramework/TextStreams/StreamWriter.hpp"
#include "TessesFramework/TextStreams/StreamReader.hpp"
#include "TessesFramework/Http/HttpStream.hpp"
#include "TessesFramework/Streams/BufferedStream.hpp"
#include <iostream>
#include <string>
using Stream = Tesses::Framework::Streams::Stream;
using NetworkStream = Tesses::Framework::Streams::NetworkStream;
using ClientTLSStream = Tesses::Framework::Crypto::ClientTLSStream;
using StreamReader = Tesses::Framework::TextStreams::StreamReader;
using StreamWriter = Tesses::Framework::TextStreams::StreamWriter;
using BufferedStream = Tesses::Framework::Streams::BufferedStream;
using HttpStream = Tesses::Framework::Http::HttpStream;
namespace Tesses::Framework::Http
{
    

    void HttpRequestBody::HandleHeaders(HttpDictionary& dict)
    {

    }

    HttpRequestBody::~HttpRequestBody()
    {

    }
    TextHttpRequestBody::TextHttpRequestBody(std::string text, std::string mimeType)
    {
        this->text = text;
        this->mimeType = mimeType;
    }
    void TextHttpRequestBody::HandleHeaders(HttpDictionary& dict)
    {
        dict.SetValue("Content-Type",this->mimeType);
        dict.SetValue("Content-Length",std::to_string(this->text.size()));
    }
    void TextHttpRequestBody::Write(Tesses::Framework::Streams::Stream* strm)
    {
        strm->WriteBlock((const uint8_t*)this->text.c_str(),this->text.size());
    }
    TextHttpRequestBody::~TextHttpRequestBody()
    {

    }

    StreamHttpRequestBody::StreamHttpRequestBody(Stream* strm, bool owns, std::string mimeType)
    {
        this->strm = strm;
        this->owns = owns;
        this->mimeType = mimeType;
    }
    void StreamHttpRequestBody::HandleHeaders(HttpDictionary& dict)
    {
        dict.AddValue("Content-Type",this->mimeType);
        auto len = this->strm->GetLength();
        if(len > -1) dict.AddValue("Content-Length",std::to_string(len));

    }
    void StreamHttpRequestBody::Write(Tesses::Framework::Streams::Stream* strm)
    {
        strm->CopyTo(this->strm);
    }
    StreamHttpRequestBody::~StreamHttpRequestBody()
    {
        if(this->owns)
            delete this->strm;
    }
    HttpRequest::HttpRequest()
    {
        this->body=nullptr;
        this->followRedirects=true;
        this->ignoreSSLErrors=false;
        this->trusted_root_cert_bundle="";
        this->method = "GET";
        this->requestHeaders.SetValue("Connection","close");
    }

    void HttpRequest::SendRequest(Tesses::Framework::Streams::Stream* strm)
    {
        Uri uri;
        if(!Uri::TryParse(this->url, uri)) return;
            
            
        if(body != nullptr)
        {
            body->HandleHeaders(requestHeaders);
        }

        std::string request = method + " " + uri.GetPathAndQuery() + " HTTP/1.1\r\nHost: " + uri.HostPort() + "\r\n";

        for(auto headers : requestHeaders.kvp)
        {
            for(auto item : headers.second)
            {
                request.append(headers.first);
                request.append(": ");
                request.append(item);
                request.append("\r\n");
            }
                
        }

        request.append("\r\n");


        StreamWriter writer(strm, false);
        writer.Write(request);

        if(body != nullptr)
        {
            body->Write(strm);
        }
    }

    Stream* HttpRequest::EstablishConnection(Uri uri, bool ignoreSSLErrors, std::string trusted_root_cert_bundle)
    {
        if(uri.scheme == "http:")
        {
            return new NetworkStream(uri.host,uri.GetPort(),false,false,false);
        }
        else if(uri.scheme == "https:")
        {
            auto netStrm = new NetworkStream(uri.host,uri.GetPort(),false,false,false);
            if(netStrm == nullptr) return netStrm;
            return new ClientTLSStream(netStrm, true, ignoreSSLErrors,uri.host, trusted_root_cert_bundle);
        }
            
        return nullptr;
    }
    
    HttpResponse::~HttpResponse()
    {
        if(this->owns)
            delete this->handleStrm;
    }

    HttpResponse::HttpResponse(Stream* strm, bool owns)
    {
        this->handleStrm=nullptr;
        this->owns=owns;
        StreamReader reader(strm, false);
            std::string statusLine;
            if(!reader.ReadLine(statusLine)) return;
            auto statusLinesPart = HttpUtils::SplitString(statusLine," ",3);
            if(statusLinesPart.size() >= 2)
            {
                this->version = statusLinesPart[0];
                this->statusCode = (StatusCode)std::stoi(statusLinesPart[1]);
            }
            std::string line;
            while(reader.ReadLine(line))
            {
                if(line.empty()) break;
                auto v = HttpUtils::SplitString(line,": ", 2);
                if(v.size() != 2) 
                {
                    delete strm;
                    return;
                }
                this->responseHeaders.AddValue(v[0],v[1]);
                line.clear();
            }
        this->handleStrm = strm;
    }

    HttpResponse::HttpResponse(HttpRequest& req)
    {
        this->owns=true;
        this->handleStrm = nullptr;
        
        std::string url = req.url;
        Uri uri;
        while(Uri::TryParse(url, uri))
        {
            auto strm = HttpRequest::EstablishConnection(uri, req.ignoreSSLErrors,req.trusted_root_cert_bundle);
            if(strm == nullptr) return;
            auto reqHeaders = req.requestHeaders;
            
            if(req.body != nullptr)
            {
                req.body->HandleHeaders(reqHeaders);
            }

            std::string request = req.method + " " + uri.GetPathAndQuery() + " HTTP/1.1\r\nHost: " + uri.HostPort() + "\r\n";

            for(auto headers : reqHeaders.kvp)
            {
                for(auto item : headers.second)
                {
                    request.append(headers.first);
                    request.append(": ");
                    request.append(item);
                    request.append("\r\n");
                }
                
            }

            request.append("\r\n");


            StreamWriter writer(strm, false);
            writer.Write(request);

            if(req.body != nullptr)
            {
                req.body->Write(strm);
            }

            StreamReader reader(strm, false);
            std::string statusLine;
            if(!reader.ReadLine(statusLine)) break;
            auto statusLinesPart = HttpUtils::SplitString(statusLine," ",3);
            if(statusLinesPart.size() >= 2)
            {
                this->version = statusLinesPart[0];
                this->statusCode = (StatusCode)std::stoi(statusLinesPart[1]);
            }
            std::string line;
            while(reader.ReadLine(line))
            {
                if(line.empty()) break;
                auto v = HttpUtils::SplitString(line,": ", 2);
                if(v.size() != 2) 
                {
                    delete strm;
                    return;
                }
                this->responseHeaders.AddValue(v[0],v[1]);
                line.clear();
            }

            std::string location;
            Uri uri2;

            
            if(req.followRedirects && (this->statusCode == MovedPermanently || this->statusCode == PermanentRedirect || this->statusCode == TemporaryRedirect) && this->responseHeaders.TryGetFirst("Location",location) && uri.Relative(location, uri2))
            {
                this->responseHeaders.Clear();
                
                url = uri2.ToString();
               
                
                delete strm;
                continue;
            }
            this->handleStrm = strm;
            break;
        }
    }
    std::string HttpResponse::ReadAsString()
    {
        auto strm = ReadAsStream();
        if(strm == nullptr) return {};
        StreamReader r(strm, true);
        return r.ReadToEnd();
    }
    void HttpResponse::CopyToStream(Stream* strm)
    {
        if(strm == nullptr) return;
        auto src = ReadAsStream();
        if(src == nullptr) return;
        src->CopyTo(*strm);
        delete src;
    }
    Stream* HttpResponse::ReadAsStream()
    {
        if(this->handleStrm == nullptr) return nullptr;
        int64_t length = -1;
        if(!this->responseHeaders.TryGetFirstInt("Content-Length",length))
        {
            length = -1;
        }

        return new HttpStream(this->handleStrm,false,length,true,version=="HTTP/1.1");
    }
}