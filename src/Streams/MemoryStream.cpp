#include "TessesFramework/Streams/MemoryStream.hpp"

namespace Tesses::Framework::Streams
{
    MemoryStream::MemoryStream(bool isWritable)
    {
        this->offset=0;
        this->writable = isWritable;
    }
    std::vector<uint8_t>& MemoryStream::GetBuffer()
    {
        return this->buffer;
    }
    size_t MemoryStream::Read(uint8_t* buff, size_t sz)
    {
        if(this->offset >= this->buffer.size()) return 0;
        size_t toRead = std::min(sz, this->buffer.size()-this->offset);
        memcpy(buff, this->buffer.data() + this->offset, toRead);
        this->offset += toRead;
        return toRead;
    }
    size_t MemoryStream::Write(const uint8_t* buff, size_t sz)
    {
        if(!this->writable) return 0;
        if(this->offset > this->buffer.size())
        {
            this->buffer.resize(this->offset+sz);
        }
        this->buffer.insert(this->buffer.begin()+this->offset, buff, buff+sz);
        this->offset+=sz;
        return sz;
    }
    bool MemoryStream::CanRead()
    {
        return true;
    }
    bool MemoryStream::CanWrite()
    {
        return this->writable;
    }
    bool MemoryStream::CanSeek()
    {
        return true;
    }
    int64_t MemoryStream::GetLength()
    {
        return this->buffer.size();
    }
    int64_t MemoryStream::GetPosition()
    {
        return (int64_t)this->offset;
    }
    void MemoryStream::Seek(int64_t pos, SeekOrigin whence)
    {
        switch(whence)
        {
            case SeekOrigin::Begin:
                this->offset = (size_t)pos;
                break;
            case SeekOrigin::Current:
                this->offset += (size_t)pos;
                break;
            case SeekOrigin::End:
                this->offset = (size_t)(this->buffer.size() + pos);
                break;
        }
    }
}