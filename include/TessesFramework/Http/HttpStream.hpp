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
#include "../Streams/Stream.hpp"

namespace Tesses::Framework::Http {
class HttpStream : public Tesses::Framework::Streams::Stream {
    std::shared_ptr<Tesses::Framework::Streams::Stream> strm;

    size_t offset;
    size_t read;
    int64_t length;
    int64_t position;

    bool recv;
    bool http1_1;

    bool done;

  public:
    HttpStream(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,
               int64_t length, bool recv, bool http1_1);
    bool CanRead();
    bool CanWrite();
    bool EndOfStream();
    int64_t GetLength();
    int64_t GetPosition();
    size_t Read(uint8_t *buffer, size_t len);
    size_t Write(const uint8_t *buffer, size_t len);
    void Close();
    ~HttpStream();
};
} // namespace Tesses::Framework::Http