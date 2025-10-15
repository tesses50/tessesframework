#pragma once
#include "../Common.hpp"
namespace Tesses::Framework::Streams
{
    enum class SeekOrigin : uint8_t {
        Begin=0,
        Current=1,
        End=2
    };
    class Stream {
        public:
            int32_t ReadByte();
            void WriteByte(uint8_t b);
            virtual bool EndOfStream();
            virtual size_t Read(uint8_t* buff, size_t sz);
            virtual size_t Write(const uint8_t* buff, size_t sz);
            size_t ReadBlock(uint8_t* buff, size_t sz);
            void WriteBlock(const uint8_t* buff, size_t sz);
            virtual bool CanRead();
            virtual bool CanWrite();
            virtual bool CanSeek();
            virtual int64_t GetPosition();
            virtual int64_t GetLength();
            virtual void Flush();
            virtual void Seek(int64_t pos, SeekOrigin whence);
            void CopyTo(std::shared_ptr<Stream> strm, size_t buffSize=1024);
            void CopyToLimit(std::shared_ptr<Stream> strm,uint64_t len, size_t buffSize=1024);
            virtual void Close();
            virtual ~Stream();
    };
}