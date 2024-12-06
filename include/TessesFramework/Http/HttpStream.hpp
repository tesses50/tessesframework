#pragma once
#include "../Streams/Stream.hpp"

namespace Tesses::Framework::Http
{
    class HttpStream : public Tesses::Framework::Streams::Stream {
        Tesses::Framework::Streams::Stream* strm;
        
        size_t offset;
        size_t read;
        int64_t length;
        int64_t position;
        
        bool owns;
        bool recv;
        bool http1_1;
        
        bool done;

        public:
            HttpStream(Tesses::Framework::Streams::Stream* strm, bool owns, int64_t length, bool recv, bool http1_1);
            HttpStream(Tesses::Framework::Streams::Stream& strm, int64_t length, bool recv,bool http1_1);
            bool CanRead();
            bool CanWrite();
            bool EndOfStream();
            int64_t GetLength();
            int64_t GetPosition();
            size_t Read(uint8_t* buffer, size_t len);
            size_t Write(const uint8_t* buffer, size_t len);
            ~HttpStream();
    };
}