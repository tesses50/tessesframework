#include "TessesFramework/Streams/ByteWriter.hpp"
namespace Tesses::Framework::Streams
{
    std::shared_ptr<Stream> ByteWriter::GetStream()
    {
        return this->strm;
    }
    ByteWriter::ByteWriter(std::shared_ptr<Stream> strm)
    {
        this->strm = strm;
    }
    
    void ByteWriter::WriteU8(uint8_t v)
    {
        strm->WriteByte(v);
    }
    void ByteWriter::WriteU16BE(uint16_t v)
    {
        uint8_t b[2];
        b[0] = (uint8_t)(v >> 8);
        b[1] = (uint8_t)v;
        strm->WriteBlock(b,2);
    }
    void ByteWriter::WriteU16LE(uint16_t v)
    {
        uint8_t b[2];
        b[0] = (uint8_t)v;
        b[1] = (uint8_t)(v >> 8);
        strm->WriteBlock(b,2);
    }
    void ByteWriter::WriteU32BE(uint32_t v)
    {
        uint8_t b[4];
        b[0] = (uint8_t)(v >> 24);
        b[1] = (uint8_t)(v >> 16);
        b[2] = (uint8_t)(v >> 8);
        b[3] = (uint8_t)v;
        strm->WriteBlock(b,4);
    }
    void ByteWriter::WriteU32LE(uint32_t v)
    {
        uint8_t b[4];
        b[0] = (uint8_t)v;
        b[1] = (uint8_t)(v >> 8);
        b[2] = (uint8_t)(v >> 16);
        b[3] = (uint8_t)(v >> 24);
    
        strm->WriteBlock(b,4);
    }
    void ByteWriter::WriteU64BE(uint64_t v)
    {
        uint8_t b[8];
        b[0] = (uint8_t)(v >> 56);
        b[1] = (uint8_t)(v >> 48);
        b[2] = (uint8_t)(v >> 40);
        b[3] = (uint8_t)(v >> 32);
        b[4] = (uint8_t)(v >> 24);
        b[5] = (uint8_t)(v >> 16);
        b[6] = (uint8_t)(v >> 8);
        b[7] = (uint8_t)v;
        strm->WriteBlock(b,8);
    }
    void ByteWriter::WriteU64LE(uint64_t v)
    {
        uint8_t b[8];
        
        b[0] = (uint8_t)v;
        b[1] = (uint8_t)(v >> 8);
        b[2] = (uint8_t)(v >> 16);
        b[3] = (uint8_t)(v >> 24);
        b[4] = (uint8_t)(v >> 32);
        b[5] = (uint8_t)(v >> 40);
        b[6] = (uint8_t)(v >> 48);
        b[7] = (uint8_t)(v >> 56);
        strm->WriteBlock(b,8);
    }
    void ByteWriter::WriteI8(int8_t v)
    {
        uint8_t data = *(uint8_t*)&v;
        WriteU8(data);
    }
    void ByteWriter::WriteI16BE(int16_t v)
    {
        uint16_t data = *(uint16_t*)&v;
        WriteU16BE(data);
    }
    void ByteWriter::WriteI16LE(int16_t v)
    {
        uint16_t data = *(uint16_t*)&v;
        WriteU16LE(data);
    }
    void ByteWriter::WriteI32BE(int32_t v)
    {
        uint32_t data = *(uint32_t*)&v;
        WriteU32BE(data);
    }
    void ByteWriter::WriteI32LE(int32_t v)
    {
        uint32_t data = *(uint32_t*)&v;
        WriteU32LE(data);
    }
    void ByteWriter::WriteI64BE(int64_t v)
    {
        uint64_t data = *(uint64_t*)&v;
        WriteU64BE(data);
    }
    void ByteWriter::WriteI64LE(int64_t v)
    {
        uint64_t data = *(uint64_t*)&v;
        WriteU64LE(data);
    }
    void ByteWriter::WriteF32BE(float v)
    {
        uint32_t data = *(uint32_t*)&v;
        WriteU32BE(data);
    }
    void ByteWriter::WriteF32LE(float v)
    {
        uint32_t data = *(uint32_t*)&v;
        WriteU32LE(data);
    }
    void ByteWriter::WriteF64BE(double v)
    {
        uint64_t data = *(uint64_t*)&v;
        WriteU64BE(data);
    }
    void ByteWriter::WriteF64LE(double v)
    {
        uint64_t data = *(uint64_t*)&v;
        WriteU64LE(data);
    }
    
}