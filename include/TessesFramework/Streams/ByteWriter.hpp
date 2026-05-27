/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
   https://git.tesses.org/tesses50/crosslang Copyright (C) 2026 Mike Nolan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "../Uuid.hpp"
#include "Stream.hpp"
namespace Tesses::Framework::Streams {
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
    void WriteUuid(const Uuid &uuid);
};
} // namespace Tesses::Framework::Streams