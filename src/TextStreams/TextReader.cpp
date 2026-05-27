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

#include "TessesFramework/TextStreams/TextReader.hpp"

namespace Tesses::Framework::TextStreams {
bool TextReader::Rewind() { return false; }
int32_t TextReader::ReadChar() {
    std::string txt;
    this->ReadBlock(txt, 1);
    if (txt.empty()) {
        eof = true;
        return -1;
    }
    return (uint8_t)txt[0];
}
std::string TextReader::ReadLine() {
    std::string str = {};
    ReadLine(str);
    return str;
}
bool TextReader::ReadLineHttp(std::string &str) {
    if (eof)
        return false;
    bool ret = false;
    int32_t r = -1;
    do {
        r = ReadChar();
        if (r == -1) {
            break;
        }
        if (r == '\r')
            continue;
        if (r == '\n')
            break;
        str.push_back((char)(uint8_t)r);
        ret = true;
    } while (r != -1);
    return ret;
}
bool TextReader::ReadLine(std::string &str) {

    if (eof)
        return false;
    bool ret = false;
    int32_t r = -1;
    do {
        r = ReadChar();
        if (r == -1)
            break;
        if (r == '\n')
        {
            if(!str.empty() && str.back() == '\r') str.resize(str.size()-1);
            return true;
        }
        str.push_back((char)(uint8_t)r);
        ret = true;
    } while (r != -1);
    
    if(!str.empty() && str.back() == '\r') str.resize(str.size()-1);
    return ret;
}
void TextReader::ReadAllLines(std::vector<std::string> &lines) {
    if (eof)
        return;
    int32_t r = -1;
    std::string builder;
    do {
        r = ReadChar();
        if (r == -1)
            break;
        
        if (r == '\n') {
            
            if(!builder.empty() && builder.back() == '\r') 
                builder.resize(builder.size()-1);


            lines.push_back(builder);
            builder.clear();
            continue;
        }
        builder += (char)r;

    } while (r != -1);
}

std::string TextReader::ReadToEnd() {
    std::string str = {};
    ReadToEnd(str);
    return str;
}

void TextReader::ReadToEnd(std::string &str) {

    if (eof)
        return;
    while (ReadBlock(str, 1024))
        ;
}
void TextReader::CopyTo(TextWriter &writer, size_t buffSz) {

    if (eof)
        return;
    std::string str = {};
    while (ReadBlock(str, buffSz)) {
        writer.Write(str);
        str.clear();
    }
}

TextReader::~TextReader() {}
} // namespace Tesses::Framework::TextStreams