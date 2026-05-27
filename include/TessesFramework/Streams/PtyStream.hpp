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
struct WindowSize {
    uint16_t Width;
    uint16_t Height;
    uint16_t Columns;
    uint16_t Rows;
};
class PtyStream : public Stream {
    int socket;
    int64_t pid;
    bool eos;
    WindowSize wS;

  public:
    PtyStream(WindowSize sz, std::string filename,
              std::vector<std::string> args, std::vector<std::string> env);
    bool EndOfStream();
    bool CanRead();
    bool CanWrite();
    size_t Read(uint8_t *buff, size_t sz);
    size_t Write(const uint8_t *buff, size_t sz);
    void Resize(WindowSize sz);
    WindowSize GetWindowSize();
    ~PtyStream();
    void Close();
};
} // namespace Tesses::Framework::Streams