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
#include "../Common.hpp"

namespace Tesses::Framework::TextStreams {
class NewLine {}; // dummy class
class TextWriter {
  public:
    TextWriter();
    std::string newline;
    virtual void WriteData(const char *text, size_t len) = 0;
    void Write(int64_t n);
    void Write(uint64_t n);
    void Write(const void *ptr);
    void Write(const char *ptr);
    void Write(char c);
    void Write(double d);
    void Write(std::string text);
    inline TextWriter &operator<<(int64_t n) {
        Write(n);
        return *this;
    }
    inline TextWriter &operator<<(uint64_t n) {
        Write(n);
        return *this;
    }
    inline TextWriter &operator<<(const void *n) {
        Write(n);
        return *this;
    }
    inline TextWriter &operator<<(const char *n) {
        Write(n);
        return *this;
    }
    inline TextWriter &operator<<(char n) {
        Write(n);
        return *this;
    }
    inline TextWriter &operator<<(double n) {
        Write(n);
        return *this;
    }
    inline TextWriter &operator<<(std::string n) {
        Write(n);
        return *this;
    }

    inline TextWriter &operator<<(NewLine nl) {
        WriteLine();
        return *this;
    }

    void WriteLine(std::string txt);
    void WriteLine(int64_t n);
    void WriteLine(uint64_t n);
    void WriteLine(const void *ptr);
    void WriteLine(const char *ptr);
    void WriteLine(char c);
    void WriteLine(double d);

    void WriteLine();

    virtual ~TextWriter();
};

} // namespace Tesses::Framework::TextStreams