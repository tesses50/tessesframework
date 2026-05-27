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
class ByteReader {
    std::shared_ptr<Stream> strm;

  public:
    std::shared_ptr<Stream> GetStream();
    ByteReader(std::shared_ptr<Stream> strm);
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
    Uuid ReadUuid();
    void ReadUuid(Uuid &uuid);
};
} // namespace Tesses::Framework::Streams