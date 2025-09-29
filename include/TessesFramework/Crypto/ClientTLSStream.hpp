#pragma once
#include "../Streams/Stream.hpp"


namespace Tesses::Framework::Crypto
{
    class ClientTLSStream : public Tesses::Framework::Streams::Stream {
        void* privateData;
        static int strm_send(void* ctx,const unsigned char* buf,size_t len);
        static int strm_recv(void* ctx,unsigned char* buf,size_t len);
        public:
            static std::string GetCertChain();

            ClientTLSStream(std::shared_ptr<Tesses::Framework::Streams::Stream> innerStream, bool verify, std::string domain);
            ClientTLSStream(std::shared_ptr<Tesses::Framework::Streams::Stream> innerStream, bool verify, std::string domain, std::string cert);
            size_t Read(uint8_t* buff, size_t sz);
            size_t Write(const uint8_t* buff, size_t sz);
            bool CanRead();
            bool CanWrite();
            bool EndOfStream();
            ~ClientTLSStream();
    };

}