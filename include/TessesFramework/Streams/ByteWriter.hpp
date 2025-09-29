#pragma once
#include "Stream.hpp"

namespace Tesses::Framework::Streams
{
    class ByteWriter {
        std::shared_ptr<Stream> strm;
        
        public:
            std::shared_ptr<Stream> GetStream();
            ByteWriter(std::shared_ptr<Stream> strm);
            void WriteU8(uint8_t v);
            void WriteU16BE(uint16_t v);
            void WriteU16LE(uint16_t v);
            void WriteU32BE(uint32_t v);
            void WriteU32LE(uint32_t v);
            void WriteU64BE(uint64_t v);
            void WriteU64LE(uint64_t v);
            void WriteI8(int8_t v);
            void WriteI16BE(int16_t v);
            void WriteI16LE(int16_t v);
            void WriteI32BE(int32_t v);
            void WriteI32LE(int32_t v);
            void WriteI64BE(int64_t v);
            void WriteI64LE(int64_t v);
            void WriteF32BE(float v);
            void WriteF32LE(float v);
            void WriteF64BE(double v);
            void WriteF64LE(double v);
    };
}