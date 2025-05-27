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
            virtual void Write(Tesses::Framework::Streams::Stream* strm)=0;
            virtual ~HttpRequestBody();
    };

    class StreamHttpRequestBody : public HttpRequestBody {
        Tesses::Framework::Streams::Stream* strm;
        bool owns;
        std::string mimeType;
        public:
            StreamHttpRequestBody(Tesses::Framework::Streams::Stream* strm, bool owns, std::string mimeType);
            void HandleHeaders(HttpDictionary& dict);
            void Write(Tesses::Framework::Streams::Stream* strm);
            ~StreamHttpRequestBody();
    };

    class TextHttpRequestBody : public HttpRequestBody {
        std::string text;
        std::string mimeType;
        public:
            TextHttpRequestBody(std::string text, std::string mimeType);
            void HandleHeaders(HttpDictionary& dict);
            void Write(Tesses::Framework::Streams::Stream* strm);
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

            static Tesses::Framework::Streams::Stream* EstablishConnection(Uri uri,bool ignoreSSLErrors,std::string trusted_root_cert_bundle);
            static Tesses::Framework::Streams::Stream* EstablishUnixPathConnection(std::string unixPath, Uri uri, bool ignoreSSLErrors, std::string trusted_root_cert_bundle);

            void SendRequest(Tesses::Framework::Streams::Stream* strm);
    };  

    class HttpResponse {
        private:
            bool owns;
            Tesses::Framework::Streams::Stream* handleStrm;
        public:
            HttpResponse(Tesses::Framework::Streams::Stream* strm, bool owns);
            HttpResponse(HttpRequest& request);
            std::string version;
            StatusCode statusCode;
            HttpDictionary responseHeaders;
            std::string ReadAsString();
            Tesses::Framework::Streams::Stream* ReadAsStream();
            void CopyToStream(Tesses::Framework::Streams::Stream* strm);
            Tesses::Framework::Streams::Stream* GetInternalStream();
            ~HttpResponse();
    };

    void DownloadToStreamSimple(std::string url, Tesses::Framework::Streams::Stream* strm);
    void DownloadToStreamSimple(std::string url, Tesses::Framework::Streams::Stream& strm);

    void DownloadToFileSimple(std::string url, Tesses::Framework::Filesystem::VFS* vfs, Tesses::Framework::Filesystem::VFSPath path);
    void DownloadToFileSimple(std::string url, Tesses::Framework::Filesystem::VFS& vfs, Tesses::Framework::Filesystem::VFSPath path);
    void DownloadToFileSimple(std::string url, Tesses::Framework::Filesystem::VFSPath path);
    std::string DownloadToStringSimple(std::string url);
    
    bool WebSocketClientSuccessDefault(HttpDictionary& dict,bool v);
    void WebSocketClient(std::string url, HttpDictionary& requestHeaders, WebSocketConnection& wsc, std::function<bool(HttpDictionary&,bool)> cb=WebSocketClientSuccessDefault);
    void WebSocketClient(std::string url, HttpDictionary& requestHeaders, WebSocketConnection* wsc, std::function<bool(HttpDictionary&,bool)> cb=WebSocketClientSuccessDefault);
    
    
    void DownloadUnixSocketToStreamSimple(std::string unixSocket,std::string url, Tesses::Framework::Streams::Stream* strm);
    void DownloadUnixSocketToStreamSimple(std::string unixSocket,std::string url, Tesses::Framework::Streams::Stream& strm);

    void DownloadUnixSocketToFileSimple(std::string unixSocket,std::string url, Tesses::Framework::Filesystem::VFS* vfs, Tesses::Framework::Filesystem::VFSPath path);
    void DownloadUnixSocketToFileSimple(std::string unixSocket,std::string url, Tesses::Framework::Filesystem::VFS& vfs, Tesses::Framework::Filesystem::VFSPath path);
    
    void DownloadUnixSocketToFileSimple(std::string unixSocket,std::string url, Tesses::Framework::Filesystem::VFSPath path);
    std::string DownloadUnixSocketToStringSimple(std::string unixSocket,std::string url);


    void WebSocketUnixSocketClient(std::string unixSocket,std::string url, HttpDictionary& requestHeaders, WebSocketConnection& wsc, std::function<bool(HttpDictionary&,bool)> cb=WebSocketClientSuccessDefault);
    void WebSocketUnixSocketClient(std::string unixSocket,std::string url,HttpDictionary& requestHeaders, WebSocketConnection* wsc, std::function<bool(HttpDictionary&,bool)> cb=WebSocketClientSuccessDefault);
}