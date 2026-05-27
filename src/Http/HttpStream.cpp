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

#include "TessesFramework/Http/HttpStream.hpp"
#include "TessesFramework/TextStreams/StreamReader.hpp"
#include "TessesFramework/TextStreams/StreamWriter.hpp"
#include <iostream>
#include <sstream>
using StreamWriter = Tesses::Framework::TextStreams::StreamWriter;
using StreamReader = Tesses::Framework::TextStreams::StreamReader;
namespace Tesses::Framework::Http {
HttpStream::HttpStream(std::shared_ptr<Tesses::Framework::Streams::Stream> strm,
                       int64_t length, bool recv, bool http1_1) {
    this->strm = strm;
    this->length = length;
    this->recv = recv;
    this->http1_1 = http1_1;
    this->offset = 0;
    this->read = 0;
    this->position = 0;
    this->done = false;
}
bool HttpStream::CanRead() {
    if (this->done)
        return false;
    if (!this->recv)
        return false;
    if (this->offset < this->read)
        return true;
    return this->strm->CanRead();
}
bool HttpStream::CanWrite() {
    if (this->done)
        return false;
    if (this->recv)
        return false;
    return this->strm->CanWrite();
}
bool HttpStream::EndOfStream() {
    if (this->done)
        return true;
    if (this->offset < this->read)
        return false;
    return this->strm->EndOfStream();
}
int64_t HttpStream::GetLength() { return this->length; }
int64_t HttpStream::GetPosition() { return this->position; }
size_t HttpStream::Read(uint8_t *buff, size_t len) {
    if (this->done)
        return 0;
    if (!this->recv)
        return 0;
    if (this->length == 0)
        return 0;
    if (this->length > 0) {

        len = std::min((size_t)(this->length - this->position), len);

        if (len > 0)
            len = this->strm->Read(buff, len);
        this->position += len;
        return len;
    } else {
        if (this->http1_1) {
            if (this->offset < this->read) {

                len = std::min((size_t)(this->read - this->offset), len);
                if (len > 0)
                    len = this->strm->Read(buff, len);
                this->offset += len;
                this->position += len;
                if (this->offset >= this->read) {
                    StreamReader reader(this->strm);
                    reader.ReadLine();
                }
                return len;
            } else {
                StreamReader reader(this->strm);
                std::string line = reader.ReadLine();
                if (!line.empty()) {
                    this->read = std::stoull(line, NULL, 16);

                    if (this->read == 0) {
                        reader.ReadLine();
                        this->done = true;
                        return 0;
                    } else {
                        this->offset = 0;

                        len =
                            std::min((size_t)(this->read - this->offset), len);
                        if (len > 0)
                            len = this->strm->Read(buff, len);
                        this->offset += len;
                        this->position += len;
                        return len;
                    }
                }
                return 0;
            }

        } else {
            return this->strm->Read(buff, len);
        }
    }
}
size_t HttpStream::Write(const uint8_t *buff, size_t len) {
    if (this->done)
        return 0;
    if (this->recv)
        return 0;
    if (this->length == 0)
        return 0;
    if (this->length > 0) {

        len = std::min((size_t)(this->length - this->position), len);

        if (len > 0)
            len = this->strm->Write(buff, len);
        this->position += len;
        return len;
    } else {
        if (len == 0)
            return 0;
        if (this->http1_1) {
            std::stringstream strm;
            strm << std::hex << len;

            StreamWriter writer(this->strm);
            writer.newline = "\r\n";
            writer.WriteLine(strm.str());

            this->strm->WriteBlock(buff, len);

            writer.WriteLine();
            return len;
        } else {
            return this->strm->Write(buff, len);
        }
    }
}
void HttpStream::Close() {
    if (this->length == -1 && this->http1_1 && !done && !this->recv) {
        this->done = true;
        try {

            StreamWriter writer(this->strm);
            writer.newline = "\r\n";
            writer.WriteLine("0");
            writer.WriteLine();
        } catch (...) {
        }
    }
}
HttpStream::~HttpStream() {
    if (this->length == -1 && this->http1_1 && !done && !this->recv) {
        try {

            StreamWriter writer(this->strm);
            writer.newline = "\r\n";
            writer.WriteLine("0");
            writer.WriteLine();
        } catch (...) {
        }
    }
}
} // namespace Tesses::Framework::Http