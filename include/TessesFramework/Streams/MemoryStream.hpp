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
#include "Stream.hpp"
namespace Tesses::Framework::Streams {
class MemoryStream : public Stream {
    std::vector<uint8_t> buffer;
    size_t offset;
    bool writable;

  public:
    MemoryStream(bool isWritable);
    std::vector<uint8_t> &GetBuffer();
    size_t Read(uint8_t *buff, size_t sz);
    size_t Write(const uint8_t *buff, size_t sz);
    bool CanRead();
    bool CanWrite();
    bool CanSeek();
    int64_t GetLength();
    int64_t GetPosition();
    void Seek(int64_t pos, SeekOrigin whence);
    void Close();
};
} // namespace Tesses::Framework::Streams