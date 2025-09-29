#pragma once
#include "../Streams/Stream.hpp"
#include "HttpUtils.hpp"
#include "WebSocket.hpp"
#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/Filesystem/VFSFix.hpp"
namespace Tesses::Framework::Http
{
    
    class HttpRequestBody {
        public:
            virtual void HandleHeaders(HttpDictionary& dict);
            virtual void Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm)=0;
            virtual ~HttpRequestBody();
    };

    class StreamHttpRequestBody : public HttpRequestBody {
        std::shared_ptr<Tesses::Framework::Streams::Stream> strm;
        
        std::string mimeType;
        public:
            StreamHttpRequestBody(std::shared_ptr<Tesses::Framework::Streams::Stream> strm, std::string mimeType);
            void HandleHeaders(HttpDictionary& dict);
            void Write(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
            ~StreamHttpRequestBody();
    };

    class TextHttpRequestBody : public HttpRequestBody {
        std::string text;
        std::string mimeType;
        public:
            TextHttpRequestBody(std::string text, std::string mimeType);
            void HandleHeaders(HttpDictionary& dict);
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
            HttpRequestBody* body;

            static std::shared_ptr<Tesses::Framework::Streams::Stream> EstablishConnection(Uri uri,bool ignoreSSLErrors,std::string trusted_root_cert_bundle);
            static std::shared_ptr<Tesses::Framework::Streams::Stream> EstablishUnixPathConnection(std::string unixPath, Uri uri, bool ignoreSSLErrors, std::string trusted_root_cert_bundle);

            void SendRequest(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
    };  

    class HttpResponse {
        private:
            bool owns;
            std::shared_ptr<Tesses::Framework::Streams::Stream> handleStrm;
        public:
            HttpResponse(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
            HttpResponse(HttpRequest& request);
            std::string version;
            StatusCode statusCode;
            HttpDictionary responseHeaders;
            std::string ReadAsString();
            std::shared_ptr<Tesses::Framework::Streams::Stream> ReadAsStream();
            void CopyToStream(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
            std::shared_ptr<Tesses::Framework::Streams::Stream> GetInternalStream();
            ~HttpResponse();
    };

    void DownloadToStreamSimple(std::string url, std::shared_ptr<Tesses::Framework::Streams::Stream> strm);

    void DownloadToFileSimple(std::string url, std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, Tesses::Framework::Filesystem::VFSPath path);

    void DownloadToFileSimple(std::string url, Tesses::Framework::Filesystem::VFSPath path);
    std::string DownloadToStringSimple(std::string url);
    
    bool WebSocketClientSuccessDefault(HttpDictionary& dict,bool v);
    void WebSocketClient(std::string url, HttpDictionary& requestHeaders, std::shared_ptr<WebSocketConnection> wsc, std::function<bool(HttpDictionary&,bool)> cb=WebSocketClientSuccessDefault);
    
    
    void DownloadUnixSocketToStreamSimple(std::string unixSocket,std::string url, std::shared_ptr<Tesses::Framework::Streams::Stream> strm);

    void DownloadUnixSocketToFileSimple(std::string unixSocket,std::string url, std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, Tesses::Framework::Filesystem::VFSPath path);
    
    void DownloadUnixSocketToFileSimple(std::string unixSocket,std::string url, Tesses::Framework::Filesystem::VFSPath path);
    std::string DownloadUnixSocketToStringSimple(std::string unixSocket,std::string url);


    void WebSocketUnixSocketClient(std::string unixSocket,std::string url, HttpDictionary& requestHeaders, std::shared_ptr<WebSocketConnection> wsc, std::function<bool(HttpDictionary&,bool)> cb=WebSocketClientSuccessDefault);
}