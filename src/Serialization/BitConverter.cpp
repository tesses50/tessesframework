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

#include "TessesFramework/Serialization/BitConverter.hpp"

namespace Tesses::Framework::Serialization {
double BitConverter::ToDoubleBits(uint64_t v) {
    static_assert(sizeof(double) == sizeof(uint64_t),
                  "double is not the same size as uint64_t");
    double dest = 0;
    memcpy(&dest, &v, sizeof(uint64_t));
    return dest;
}
uint64_t BitConverter::ToUintBits(double v) {
    // as static_assert is compile time we don't need it here
    uint64_t dest = 0;
    memcpy(&dest, &v, sizeof(uint64_t));
    return dest;
}
float BitConverter::ToFloatBits(uint32_t v) {
    static_assert(sizeof(float) == sizeof(uint32_t),
                  "float is not the same size as uint32_t");
    float dest = 0;
    memcpy(&dest, &v, sizeof(uint32_t));
    return dest;
}
uint32_t BitConverter::ToUint32Bits(float v) {
    // as static_assert is compile time we don't need it here
    uint32_t dest = 0;
    memcpy(&dest, &v, sizeof(uint32_t));
    return dest;
}
double BitConverter::ToDoubleBE(uint8_t &b) {
    return ToDoubleBits(ToUint64BE(b));
}
float BitConverter::ToFloatBE(uint8_t &b) { return ToFloatBits(ToUint32BE(b)); }
uint64_t BitConverter::ToUint64BE(uint8_t &b) {
    uint8_t *b2 = &b;
    uint64_t v = 0;
    v |= ((uint64_t)b2[0] << 56);
    v |= ((uint64_t)b2[1] << 48);
    v |= ((uint64_t)b2[2] << 40);
    v |= ((uint64_t)b2[3] << 32);
    v |= ((uint64_t)b2[4] << 24);
    v |= ((uint64_t)b2[5] << 16);
    v |= ((uint64_t)b2[6] << 8);
    v |= (uint64_t)b2[7];
    return v;
}
uint32_t BitConverter::ToUint32BE(uint8_t &b) {
    uint8_t *b2 = &b;
    uint32_t v = 0;

    v |= ((uint32_t)b2[0] << 24);
    v |= ((uint32_t)b2[1] << 16);
    v |= ((uint32_t)b2[2] << 8);
    v |= (uint32_t)b2[3];
    return v;
}
uint16_t BitConverter::ToUint16BE(uint8_t &b) {
    uint8_t *b2 = &b;
    uint16_t v = 0;

    v |= ((uint16_t)b2[0] << 8);
    v |= (uint16_t)b2[1];
    return v;
}
double BitConverter::ToDoubleLE(uint8_t &b) {
    return ToDoubleBits(ToUint64LE(b));
}
float BitConverter::ToFloatLE(uint8_t &b) { return ToFloatBits(ToUint32LE(b)); }
uint64_t BitConverter::ToUint64LE(uint8_t &b) {
    uint8_t *b2 = &b;
    uint64_t v = 0;
    v |= (uint64_t)b2[0];
    v |= ((uint64_t)b2[1] << 8);
    v |= ((uint64_t)b2[2] << 16);
    v |= ((uint64_t)b2[3] << 24);
    v |= ((uint64_t)b2[4] << 32);
    v |= ((uint64_t)b2[5] << 40);
    v |= ((uint64_t)b2[6] << 48);
    v |= ((uint64_t)b2[7] << 56);

    return v;
}
uint32_t BitConverter::ToUint32LE(uint8_t &b) {
    uint8_t *b2 = &b;
    uint32_t v = 0;
    v |= (uint32_t)b2[0];
    v |= ((uint32_t)b2[1] << 8);
    v |= ((uint32_t)b2[2] << 16);
    v |= ((uint32_t)b2[3] << 24);

    return v;
}
uint16_t BitConverter::ToUint16LE(uint8_t &b) {
    uint8_t *b2 = &b;
    uint16_t v = 0;

    v |= (uint16_t)b2[0];
    v |= ((uint16_t)b2[1] << 8);

    return v;
}
void BitConverter::FromDoubleBE(uint8_t &b, double v) {
    FromUint64BE(b, ToUintBits(v));
}
void BitConverter::FromUint64BE(uint8_t &b, uint64_t v) {
    uint8_t *b2 = &b;
    b2[0] = (uint8_t)(v >> 56);
    b2[1] = (uint8_t)(v >> 48);
    b2[2] = (uint8_t)(v >> 40);
    b2[3] = (uint8_t)(v >> 32);
    b2[4] = (uint8_t)(v >> 24);
    b2[5] = (uint8_t)(v >> 16);
    b2[6] = (uint8_t)(v >> 8);
    b2[7] = (uint8_t)v;
}
void BitConverter::FromUint32BE(uint8_t &b, uint32_t v) {
    uint8_t *b2 = &b;

    b2[0] = (uint8_t)(v >> 24);
    b2[1] = (uint8_t)(v >> 16);
    b2[2] = (uint8_t)(v >> 8);
    b2[3] = (uint8_t)v;
}
void BitConverter::FromUint16BE(uint8_t &b, uint16_t v) {
    uint8_t *b2 = &b;

    b2[0] = (uint8_t)(v >> 8);
    b2[1] = (uint8_t)v;
}

void BitConverter::FromDoubleLE(uint8_t &b, double v) {
    FromUint64BE(b, ToUintBits(v));
}
void BitConverter::FromFloatLE(uint8_t &b, float v) {
    FromUint32LE(b, ToUint32Bits(v));
}
void BitConverter::FromFloatBE(uint8_t &b, float v) {
    FromUint32BE(b, ToUint32Bits(v));
}
void BitConverter::FromUint64LE(uint8_t &b, uint64_t v) {
    uint8_t *b2 = &b;
    b2[0] = (uint8_t)v;
    b2[1] = (uint8_t)(v >> 8);
    b2[2] = (uint8_t)(v >> 16);
    b2[3] = (uint8_t)(v >> 24);
    b2[4] = (uint8_t)(v >> 32);
    b2[5] = (uint8_t)(v >> 40);
    b2[6] = (uint8_t)(v >> 48);
    b2[7] = (uint8_t)(v >> 56);
}
void BitConverter::FromUint32LE(uint8_t &b, uint32_t v) {
    uint8_t *b2 = &b;

    b2[0] = (uint8_t)v;
    b2[1] = (uint8_t)(v >> 8);
    b2[2] = (uint8_t)(v >> 16);
    b2[3] = (uint8_t)(v >> 24);
}
void BitConverter::FromUint16LE(uint8_t &b, uint16_t v) {
    uint8_t *b2 = &b;

    b2[0] = (uint8_t)v;
    b2[1] = (uint8_t)(v >> 8);
}
void BitConverter::FromUuid(uint8_t &b, const Uuid &uuid) {
    uint8_t *b2 = &b;
    FromUint32BE(b2[0], uuid.time_low);
    FromUint16BE(b2[4], uuid.time_mid);
    FromUint16BE(b2[6], uuid.time_hi_and_version);
    b2[8] = uuid.clock_seq_hi_and_reserved;
    b2[9] = uuid.clock_seq_low;
    for (size_t i = 0; i < 6; i++)
        b2[i + 10] = uuid.node[i];
}

Uuid BitConverter::ToUuid(uint8_t &b) {
    Uuid uuid;
    BitConverter::ToUuid(b, uuid);
    return uuid;
}

void BitConverter::ToUuid(uint8_t &b, Uuid &uuid) {
    uint8_t *b2 = &b;
    uuid.time_low = ToUint32BE(b2[0]);

    uuid.time_mid = ToUint16BE(b2[4]);

    uuid.time_hi_and_version = ToUint16BE(b2[6]);

    uuid.clock_seq_hi_and_reserved = b2[8];
    uuid.clock_seq_low = b2[9];
    for (size_t i = 0; i < 6; i++)
        uuid.node[i] = b2[i + 10];
}

int64_t BitConverter::ToSint64BE(uint8_t &b) {
    uint64_t src = ToUint64BE(b);
    int64_t dest = 0;
    memcpy(&dest, &src, sizeof(uint64_t));
    return dest;
}
int64_t BitConverter::ToSint64LE(uint8_t &b) {
    uint64_t src = ToUint64LE(b);
    int64_t dest = 0;
    memcpy(&dest, &src, sizeof(uint64_t));
    return dest;
}

int32_t BitConverter::ToSint32BE(uint8_t &b) {
    uint32_t src = ToUint32BE(b);
    int32_t dest = 0;
    memcpy(&dest, &src, sizeof(uint32_t));
    return dest;
}
int32_t BitConverter::ToSint32LE(uint8_t &b) {
    uint32_t src = ToUint32LE(b);
    int32_t dest = 0;
    memcpy(&dest, &src, sizeof(uint32_t));
    return dest;
}

int16_t BitConverter::ToSint16BE(uint8_t &b) {
    uint16_t src = ToUint16BE(b);
    int16_t dest = 0;
    memcpy(&dest, &src, sizeof(uint16_t));
    return dest;
}
int16_t BitConverter::ToSint16LE(uint8_t &b) {
    uint16_t src = ToUint16LE(b);
    int16_t dest = 0;
    memcpy(&dest, &src, sizeof(uint16_t));
    return dest;
}

void BitConverter::FromSint64BE(uint8_t &b, int64_t v) {
    uint64_t dest = 0;
    memcpy(&dest, &v, sizeof(uint64_t));
    FromUint64BE(b, dest);
}
void BitConverter::FromSint32BE(uint8_t &b, int32_t v) {
    uint32_t dest = 0;
    memcpy(&dest, &v, sizeof(uint32_t));
    FromUint32BE(b, dest);
}
void BitConverter::FromSint16BE(uint8_t &b, int16_t v) {
    uint16_t dest = 0;
    memcpy(&dest, &v, sizeof(uint16_t));
    FromUint16BE(b, dest);
}

void BitConverter::FromSint64LE(uint8_t &b, int64_t v) {
    uint64_t dest = 0;
    memcpy(&dest, &v, sizeof(uint64_t));
    FromUint64LE(b, dest);
}
void BitConverter::FromSint32LE(uint8_t &b, int32_t v) {
    uint32_t dest = 0;
    memcpy(&dest, &v, sizeof(uint32_t));
    FromUint32LE(b, dest);
}
void BitConverter::FromSint16LE(uint8_t &b, int16_t v) {
    uint16_t dest = 0;
    memcpy(&dest, &v, sizeof(uint16_t));
    FromUint16LE(b, dest);
}
} // namespace Tesses::Framework::Serialization