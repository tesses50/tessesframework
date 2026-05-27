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

#include "TessesFramework/TextStreams/TextWriter.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"

namespace Tesses::Framework::TextStreams {
void TextWriter::Write(int64_t n) {
    std::string text = std::to_string(n);
    WriteData(text.c_str(), text.size());
}
void TextWriter::Write(uint64_t n) {
    std::string text = std::to_string(n);
    WriteData(text.c_str(), text.size());
}

void TextWriter::Write(const void *ptr) {
    std::string text = "0x";
    uintptr_t ptr2 = (uintptr_t)ptr;

    for (size_t i = 1; i <= sizeof(ptr); i++) {
        uint8_t v = (uint8_t)(ptr2 >> (int)((sizeof(ptr) - i) * 8));
        text.push_back(Tesses::Framework::Http::HttpUtils::NibbleToHex(v >> 4));
        text.push_back(Tesses::Framework::Http::HttpUtils::NibbleToHex(v));
    }
    WriteData(text.c_str(), text.size());
}
void TextWriter::Write(const char *ptr) { WriteData(ptr, strlen(ptr)); }
void TextWriter::Write(char c) { WriteData(&c, 1); }
void TextWriter::Write(double d) {
    std::string text = std::to_string(d);
    WriteData(text.c_str(), text.size());
}

void TextWriter::WriteLine(int64_t n) {
    std::string text = std::to_string(n);
    text.append(newline);
    WriteData(text.c_str(), text.size());
}
void TextWriter::WriteLine(uint64_t n) {
    std::string text = std::to_string(n);
    text.append(newline);
    WriteData(text.c_str(), text.size());
}
void TextWriter::WriteLine(const void *ptr) {
    std::string text = "0x";
    uintptr_t ptr2 = (uintptr_t)ptr;

    for (size_t i = 1; i <= sizeof(ptr); i++) {
        uint8_t v = (uint8_t)(ptr2 >> (int)((sizeof(ptr) - i) * 8));
        text.push_back(Tesses::Framework::Http::HttpUtils::NibbleToHex(v >> 4));
        text.push_back(Tesses::Framework::Http::HttpUtils::NibbleToHex(v));
    }
    text.append(newline);
    WriteData(text.c_str(), text.size());
}
void TextWriter::WriteLine(const char *ptr) {
    std::string text = ptr;
    text.append(newline);
    WriteData(text.c_str(), text.size());
}
void TextWriter::WriteLine(char c) {
    std::string text = {c};
    text.append(newline);
    WriteData(text.c_str(), text.size());
}
void TextWriter::WriteLine(double d) {
    std::string text = std::to_string(d);
    text.append(newline);
    WriteData(text.c_str(), text.size());
}
TextWriter::TextWriter() {
#if defined(WIN32) || defined(_WIN32)
    newline = "\r\n";
#else
    newline = "\n";
#endif
}
void TextWriter::Write(std::string txt) { WriteData(txt.c_str(), txt.size()); }
void TextWriter::WriteLine(std::string txt) {
    std::string str = txt;
    str.append(newline);
    Write(str);
}
void TextWriter::WriteLine() { Write(newline); }
TextWriter::~TextWriter() {}
} // namespace Tesses::Framework::TextStreams