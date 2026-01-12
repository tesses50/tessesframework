#include "TessesFramework/Streams/Stream.hpp"
#include <iostream>

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
            {
                read=this->Write(buffer,read);
                if(read == 0)
                {
                    throw std::out_of_range("Failed to write!");
                }
            }
        

            buffer += read;
            len -= read;
        } while(read > 0 && !this->EndOfStream());
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
    void Stream::Close()
    {
        
    }

    void Stream::CopyToLimit(std::shared_ptr<Stream> strm,uint64_t len, size_t buffSize)
    {
        size_t read;
        uint8_t* buffer = new uint8_t[buffSize];
        uint64_t offset = 0;

        do {
            if(offset >= len) break;
            read = (size_t)std::min(len-offset,(uint64_t)buffSize);

            read = this->Read(buffer,read);
            strm->WriteBlock(buffer, read);

            offset += read;
            
        } while(read > 0 && !strm->EndOfStream());
        strm->Flush();

        delete[] buffer;
    }
   
    void Stream::CopyTo(std::shared_ptr<Stream> strm, size_t buffSize)
    {
        size_t read;
        uint8_t* buffer = new uint8_t[buffSize];
        do {
            read = this->Read(buffer,buffSize);
            strm->WriteBlock(buffer, read);
            
        } while(read > 0 && !strm->EndOfStream());
        strm->Flush();

        delete[] buffer;
      
    }
    Stream::~Stream()
    {

    }
}
