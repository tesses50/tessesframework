#include "TessesFramework/Streams/BufferedStream.hpp"
namespace Tesses::Framework::Streams {
    BufferedStream::BufferedStream(std::shared_ptr<Stream> strm, size_t bufferSize)
    {
        this->strm = strm;
        this->bufferSize = bufferSize;
        this->buffer = new uint8_t[bufferSize];
        this->read = 0;
        this->offset = 0;
    }
    
    bool BufferedStream::EndOfStream()
    {
        if(this->offset < this->read) return false;
        return this->strm->EndOfStream();
    }
    bool BufferedStream::CanRead()
    {
        if(this->offset < this->read) return true;
        return this->strm->CanRead();
    }
    bool BufferedStream::CanWrite()
    {
        return this->strm->CanWrite();
    }
    size_t BufferedStream::Read(uint8_t* buff, size_t sz)
    {
        if(this->offset < this->read)
        { 
            sz = std::min(sz,this->read-this->offset);
            
            memcpy(buff, this->buffer+this->offset, sz);
            this->offset+=sz;
            return sz;
        }
        if(sz < this->bufferSize)
        {
            this->read = this->strm->Read(this->buffer, this->bufferSize);
            this->offset=0;
            
            sz = std::min(sz,this->read-this->offset);
            
            memcpy(buff, this->buffer+this->offset, sz);
            this->offset+=sz;
            return sz;
        }
        else
        {
            return this->strm->Read(buff, sz);
        }
    }
    size_t BufferedStream::Write(const uint8_t* buff, size_t sz)
    {
        return this->strm->Write(buff,sz);
    }   

    BufferedStream::~BufferedStream()
    {
        delete buffer;
    }

    void BufferedStream::Close()
    {
        this->strm->Close();
    }
}