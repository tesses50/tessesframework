#pragma once
#include "Stream.hpp"

namespace Tesses::Framework::Streams
{
    class BufferedStream : public Stream
    {
        private:
            Stream* strm;
            bool owns;
            uint8_t* buffer;
            size_t bufferSize;
            size_t offset;
            size_t read;
        public:
            BufferedStream(Stream* strm, bool owns, size_t bufferSize=1024);
            BufferedStream(Stream& strm, size_t bufferSize=1024);
            bool EndOfStream();
            bool CanRead();
            bool CanWrite();
            size_t Read(uint8_t* buff, size_t sz);
            size_t Write(const uint8_t* buff, size_t sz);

            ~BufferedStream();
    };

}