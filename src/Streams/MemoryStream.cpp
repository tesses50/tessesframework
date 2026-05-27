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

#include "TessesFramework/Streams/MemoryStream.hpp"

namespace Tesses::Framework::Streams {
MemoryStream::MemoryStream(bool isWritable) {
    this->offset = 0;
    this->writable = isWritable;
}
std::vector<uint8_t> &MemoryStream::GetBuffer() { return this->buffer; }
size_t MemoryStream::Read(uint8_t *buff, size_t sz) {
    if (this->offset >= this->buffer.size())
        return 0;

    size_t toRead = std::min(sz, this->buffer.size() - this->offset);

    memcpy(buff, this->buffer.data() + this->offset, toRead);
    this->offset += toRead;
    return toRead;
}
size_t MemoryStream::Write(const uint8_t *buff, size_t sz) {
    if (!this->writable)
        return 0;
    if (this->offset > this->buffer.size()) {
        this->buffer.resize(this->offset + sz);
    }
    this->buffer.insert(this->buffer.begin() + this->offset, buff, buff + sz);
    this->offset += sz;
    return sz;
}
bool MemoryStream::CanRead() { return true; }
bool MemoryStream::CanWrite() { return this->writable; }
bool MemoryStream::CanSeek() { return true; }
int64_t MemoryStream::GetLength() { return this->buffer.size(); }
int64_t MemoryStream::GetPosition() { return (int64_t)this->offset; }
void MemoryStream::Seek(int64_t pos, SeekOrigin whence) {
    switch (whence) {
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
void MemoryStream::Close() { this->buffer.clear(); }
} // namespace Tesses::Framework::Streams