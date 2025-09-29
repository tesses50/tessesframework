#pragma once
#include "Stream.hpp"
namespace Tesses::Framework::Streams
{
    class MemoryStream : public Stream {
        std::vector<uint8_t> buffer;
        size_t offset;
        bool writable;
        public:
            MemoryStream(bool isWritable);
            std::vector<uint8_t>& GetBuffer();
            size_t Read(uint8_t* buff, size_t sz);
            size_t Write(const uint8_t* buff, size_t sz);
            bool CanRead();
            bool CanWrite();
            bool CanSeek();
            int64_t GetLength();
            int64_t GetPosition();
            void Seek(int64_t pos, SeekOrigin whence);
            void Close();
    };
}