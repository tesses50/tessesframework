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

#include "TessesFramework/TextStreams/StreamReader.hpp"
#include "TessesFramework/Streams/FileStream.hpp"
using Stream = Tesses::Framework::Streams::Stream;
using FileStream = Tesses::Framework::Streams::FileStream;

namespace Tesses::Framework::TextStreams {

StreamReader::StreamReader(std::filesystem::path path)
    : StreamReader(std::make_shared<FileStream>(path, "rb")) {}
bool StreamReader::Rewind() {
    if (this->strm->CanSeek()) {
        this->strm->Seek((int64_t)0,
                         Tesses::Framework::Streams::SeekOrigin::Begin);
        return true;
    }
    return false;
}
StreamReader::StreamReader(std::shared_ptr<Stream> strm) : TextReader() {
    this->strm = strm;
}

std::shared_ptr<Stream> StreamReader::GetStream() { return (this->strm); }

bool StreamReader::ReadBlock(std::string &str, size_t len) {
    std::vector<uint8_t> buff(len);

    len = strm->ReadBlock(buff.data(), len);
    if (len == 0) {
        return false;
    }
    str.append((const char *)buff.data(), len);

    return true;
}
StreamReader::~StreamReader() {}
}; // namespace Tesses::Framework::TextStreams