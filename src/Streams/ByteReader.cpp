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

#include "TessesFramework/Streams/ByteReader.hpp"
#include "TessesFramework/Serialization/BitConverter.hpp"
namespace Tesses::Framework::Streams {
std::shared_ptr<Stream> ByteReader::GetStream() { return this->strm; }
ByteReader::ByteReader(std::shared_ptr<Stream> strm) { this->strm = strm; }

uint8_t ByteReader::ReadU8() {
    auto r = this->strm->ReadByte();
    if (r < 0)
        throw std::runtime_error("End of file");
    return (uint8_t)r;
}
uint16_t ByteReader::ReadU16BE() {
    uint8_t data[2];
    if (this->strm->ReadBlock(data, 2) != 2)
        throw std::runtime_error("End of file");
    uint16_t n = 0;
    n |= (uint16_t)data[0] << 8;
    n |= (uint16_t)data[1];

    return n;
}
uint16_t ByteReader::ReadU16LE() {
    uint8_t data[2];
    if (this->strm->ReadBlock(data, 2) != 2)
        throw std::runtime_error("End of file");
    uint16_t n = 0;
    n |= (uint16_t)data[0];
    n |= (uint16_t)data[1] << 8;

    return n;
}
uint32_t ByteReader::ReadU32BE() {

    uint8_t data[4];
    if (this->strm->ReadBlock(data, 4) != 4)
        throw std::runtime_error("End of file");
    uint32_t n = 0;
    n |= (uint32_t)data[0] << 24;
    n |= (uint32_t)data[1] << 16;
    n |= (uint32_t)data[2] << 8;
    n |= (uint32_t)data[3];

    return n;
}
uint32_t ByteReader::ReadU32LE() {
    uint8_t data[4];
    if (this->strm->ReadBlock(data, 4) != 4)
        throw std::runtime_error("End of file");
    uint32_t n = 0;
    n |= (uint32_t)data[0];
    n |= (uint32_t)data[1] << 8;
    n |= (uint32_t)data[2] << 16;
    n |= (uint32_t)data[3] << 24;
    return n;
}
uint64_t ByteReader::ReadU64BE() {
    uint8_t data[8];
    if (this->strm->ReadBlock(data, 8) != 8)
        throw std::runtime_error("End of file");
    uint64_t n = 0;
    n |= (uint64_t)data[0] << 56;
    n |= (uint64_t)data[1] << 48;
    n |= (uint64_t)data[2] << 40;
    n |= (uint64_t)data[3] << 32;
    n |= (uint64_t)data[4] << 24;
    n |= (uint64_t)data[5] << 16;
    n |= (uint64_t)data[6] << 8;
    n |= (uint64_t)data[7];

    return n;
}
uint64_t ByteReader::ReadU64LE() {
    uint8_t data[8];
    if (this->strm->ReadBlock(data, 8) != 8)
        throw std::runtime_error("End of file");
    uint64_t n = 0;
    n |= (uint64_t)data[0];
    n |= (uint64_t)data[1] << 8;
    n |= (uint64_t)data[2] << 16;
    n |= (uint64_t)data[3] << 24;
    n |= (uint64_t)data[4] << 32;
    n |= (uint64_t)data[5] << 40;
    n |= (uint64_t)data[6] << 48;
    n |= (uint64_t)data[7] << 56;

    return n;
}
int8_t ByteReader::ReadI8() {
    auto v = ReadU8();
    return *(int8_t *)&v;
}
int16_t ByteReader::ReadI16BE() {
    auto v = ReadU16BE();
    return *(int16_t *)&v;
}
int16_t ByteReader::ReadI16LE() {
    auto v = ReadU16BE();
    return *(int16_t *)&v;
}
int32_t ByteReader::ReadI32BE() {
    auto v = ReadU32BE();
    return *(int32_t *)&v;
}
int32_t ByteReader::ReadI32LE() {
    auto v = ReadU32LE();
    return *(int32_t *)&v;
}
int64_t ByteReader::ReadI64BE() {
    auto v = ReadU64BE();
    return *(int64_t *)&v;
}
int64_t ByteReader::ReadI64LE() {
    auto v = ReadU64LE();
    return *(int64_t *)&v;
}
float ByteReader::ReadF32BE() {
    auto v = ReadU32BE();
    return *(float *)&v;
}
float ByteReader::ReadF32LE() {
    auto v = ReadU32LE();
    return *(float *)&v;
}
double ByteReader::ReadF64BE() {
    auto v = ReadU64BE();
    return *(double *)&v;
}
double ByteReader::ReadF64LE() {
    auto v = ReadU64LE();
    return *(double *)&v;
}

Uuid ByteReader::ReadUuid() {
    uint8_t data[16];
    if (this->strm->ReadBlock(data, 16) != 16)
        throw std::runtime_error("End of file");
    return Serialization::BitConverter::ToUuid(data[0]);
}
void ByteReader::ReadUuid(Uuid &uuid) {
    uint8_t data[16];
    if (this->strm->ReadBlock(data, 16) != 16)
        throw std::runtime_error("End of file");
    Serialization::BitConverter::ToUuid(data[0], uuid);
}

} // namespace Tesses::Framework::Streams