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

#include "TessesFramework/Streams/BufferedStream.hpp"
namespace Tesses::Framework::Streams {
BufferedStream::BufferedStream(std::shared_ptr<Stream> strm,
                               size_t bufferSize) {
    this->strm = strm;
    this->bufferSize = bufferSize;
    this->buffer = new uint8_t[bufferSize];
    this->read = 0;
    this->offset = 0;
}

bool BufferedStream::EndOfStream() {
    if (this->offset < this->read)
        return false;
    return this->strm->EndOfStream();
}
bool BufferedStream::CanRead() {
    if (this->offset < this->read)
        return true;
    return this->strm->CanRead();
}
bool BufferedStream::CanWrite() { return this->strm->CanWrite(); }
size_t BufferedStream::Read(uint8_t *buff, size_t sz) {
    if (this->offset < this->read) {
        sz = std::min(sz, this->read - this->offset);

        memcpy(buff, this->buffer + this->offset, sz);
        this->offset += sz;
        return sz;
    }
    if (sz < this->bufferSize) {
        this->read = this->strm->Read(this->buffer, this->bufferSize);
        this->offset = 0;

        sz = std::min(sz, this->read - this->offset);

        memcpy(buff, this->buffer + this->offset, sz);
        this->offset += sz;
        return sz;
    } else {
        return this->strm->Read(buff, sz);
    }
}
size_t BufferedStream::Write(const uint8_t *buff, size_t sz) {
    return this->strm->Write(buff, sz);
}

BufferedStream::~BufferedStream() { delete buffer; }

void BufferedStream::Close() { this->strm->Close(); }
} // namespace Tesses::Framework::Streams