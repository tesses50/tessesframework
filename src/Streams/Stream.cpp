#include "TessesFramework/Streams/Stream.hpp"

namespace Tesses::Framework::Streams {
    int32_t Stream::ReadByte()
    {
        uint8_t b;
        if(Read(&b, 1) == 0) return -1;
        return b;
    }
    void Stream::WriteByte(uint8_t b)
    {
        Write(&b, 1);
    }
    size_t Stream::Read(uint8_t* buffer, size_t count)
    {
        return 0;
    }
    size_t Stream::Write(const uint8_t* buffer, size_t count)
    {
        return 0;
    }
    size_t Stream::ReadBlock(uint8_t* buffer,size_t len)
    {
        size_t read;
        size_t readTotal = 0;
        do{
            read = 1024;
            if(len < 1024)
                read = len;
            if(read > 0)
                read=this->Read(buffer,read);
            


            buffer += read;
            len -= read;
            readTotal += read;
        } while(read > 0);
        return readTotal;
    }

    void Stream::WriteBlock(const uint8_t* buffer,size_t len)
    {
        size_t read;
        do{
            read = 1024;
            if(len < 1024)
                read = len;
            if(read > 0)
                read=this->Write(buffer,read);
           
        

            buffer += read;
            len -= read;
        } while(read > 0);
    }
    bool Stream::CanRead()
    {
        return false;
    }
    bool Stream::CanWrite()
    {
        return false;
    }   
    bool Stream::CanSeek()
    {
        return false;
    }
    bool Stream::EndOfStream()
    {
        return false;
    }
    int64_t Stream::GetPosition()
    {
        return 0;
    }
    int64_t Stream::GetLength()
    {
        if(!CanSeek()) return 0;
        int64_t curPos = GetPosition();
        Seek(0, SeekOrigin::End);
        int64_t len = GetPosition();
        Seek(curPos, SeekOrigin::Begin);
        return len;
    }
    void Stream::Flush()
    {

    }
    void Stream::Seek(int64_t pos, SeekOrigin whence)
    {

    }
    void Stream::CopyTo(Stream* strm, size_t buffSize)
    {
        if(strm != nullptr)
        this->CopyTo(*strm, buffSize);
    }
    void Stream::CopyTo(Stream& strm, size_t buffSize)
    {
        size_t read;
        uint8_t buffer[buffSize];
        do {
            read = this->Read(buffer,buffSize);
            strm.WriteBlock(buffer, read);
            
        } while(read > 0);
        strm.Flush();
    }
    Stream::~Stream()
    {

    }
}
