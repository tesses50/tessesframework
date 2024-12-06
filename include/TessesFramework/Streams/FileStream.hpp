#pragma once
#include "Stream.hpp"
#include <cstdio>
namespace Tesses::Framework::Streams
{
    class FileStream : public Stream {
        bool canRead;
        bool canWrite;
        bool canSeek;
        bool owns;
        FILE* f;
        void SetMode(std::string mode);
        public:
            FileStream(std::filesystem::path p, std::string mode);
            FileStream(FILE* f, bool owns, std::string mode , bool canSeek=true);
            size_t Read(uint8_t* buff, size_t sz);
            size_t Write(const uint8_t* buff, size_t sz);
            bool CanRead();
            bool CanWrite();
            bool CanSeek();
            int64_t GetPosition();
            void Flush();
            void Seek(int64_t pos, SeekOrigin whence);
            ~FileStream();
   
    };
}