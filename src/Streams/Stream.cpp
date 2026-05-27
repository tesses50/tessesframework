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

#include "TessesFramework/Streams/Stream.hpp"
#include <iostream>

namespace Tesses::Framework::Streams {
int32_t Stream::ReadByte() {
    uint8_t b;
    if (Read(&b, 1) == 0)
        return -1;
    return b;
}
void Stream::WriteByte(uint8_t b) { Write(&b, 1); }
size_t Stream::Read(uint8_t *buffer, size_t count) { return 0; }
size_t Stream::Write(const uint8_t *buffer, size_t count) { return 0; }
size_t Stream::ReadBlock(uint8_t *buffer, size_t len) {
    size_t read;
    size_t readTotal = 0;
    do {
        read = 1024;
        if (len < 1024)
            read = len;
        if (read > 0)
            read = this->Read(buffer, read);

        buffer += read;
        len -= read;
        readTotal += read;
    } while (read > 0);
    return readTotal;
}

void Stream::WriteBlock(const uint8_t *buffer, size_t len) {
    size_t read;
    do {
        read = 1024;
        if (len < 1024)
            read = len;
        if (read > 0) {
            size_t r0 = read;
            read = this->Write(buffer, read);

            if (read == 0) {
                throw std::out_of_range("Failed to write!");
            }
        }

        buffer += read;
        len -= read;
    } while (read > 0 && !this->EndOfStream());
}
bool Stream::CanRead() { return false; }
bool Stream::CanWrite() { return false; }
bool Stream::CanSeek() { return false; }
bool Stream::EndOfStream() { return false; }
int64_t Stream::GetPosition() { return 0; }
int64_t Stream::GetLength() {
    if (!CanSeek())
        return 0;
    int64_t curPos = GetPosition();
    Seek(0, SeekOrigin::End);
    int64_t len = GetPosition();
    Seek(curPos, SeekOrigin::Begin);
    return len;
}
void Stream::Flush() {}
void Stream::Seek(int64_t pos, SeekOrigin whence) {}
void Stream::Close() {}

void Stream::CopyToLimit(std::shared_ptr<Stream> strm, uint64_t len,
                         size_t buffSize) {
    size_t read;
    std::vector<uint8_t> buffer(buffSize);
    uint64_t offset = 0;

    do {
        if (offset >= len)
            break;
        read = (size_t)std::min(len - offset, (uint64_t)buffer.size());

        read = this->Read(buffer.data(), read);
        strm->WriteBlock(buffer.data(), read);

        offset += read;

    } while (read > 0 && !strm->EndOfStream());
    strm->Flush();
}

void Stream::CopyTo(std::shared_ptr<Stream> strm, size_t buffSize) {
    size_t read;
    std::vector<uint8_t> buffer(buffSize);
    do {
        read = this->Read(buffer.data(), buffer.size());
        strm->WriteBlock(buffer.data(), read);

    } while (read > 0 && !strm->EndOfStream());
    strm->Flush();
}
Stream::~Stream() {}
} // namespace Tesses::Framework::Streams
