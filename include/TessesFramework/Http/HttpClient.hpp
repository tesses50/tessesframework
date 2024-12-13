#pragma once
#include "../Streams/Stream.hpp"
#include "HttpUtils.hpp"

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




    class HttpRequest {
        public:
            HttpRequest();
            std::string trusted_root_cert_bundle;
            bool ignoreSSLErrors;
            bool followRedirects;

            std::string method;
            std::string url;
            HttpDictionary requestHeaders;
            HttpRequestBody* body;

            static Tesses::Framework::Streams::Stream* EstablishConnection(Uri uri,bool ignoreSSLErrors,std::string trusted_root_cert_bundle);

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
            
            ~HttpResponse();
    };

    
}