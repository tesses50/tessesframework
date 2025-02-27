#pragma once
#include "Stream.hpp"

namespace Tesses::Framework::Streams
{
    class ByteReader {
        Stream* strm;
        bool owns;
        public:
            Stream* GetStream();
            ByteReader(Stream* strm, bool owns);
            ByteReader(Stream& strm);
            uint8_t ReadU8();
            uint16_t ReadU16BE();
            uint16_t ReadU16LE();
            uint32_t ReadU32BE();
            uint32_t ReadU32LE();
            uint64_t ReadU64BE();
            uint64_t ReadU64LE();
            int8_t ReadI8();
            int16_t ReadI16BE();
            int16_t ReadI16LE();
            int32_t ReadI32BE();
            int32_t ReadI32LE();
            int64_t ReadI64BE();
            int64_t ReadI64LE();
            float ReadF32BE();
            float ReadF32LE();
            double ReadF64BE();
            double ReadF64LE();
            ~ByteReader();
    };
}