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
#include "../Streams/MemoryStream.hpp"
#include "../TextStreams/StringWriter.hpp"

namespace Tesses::Framework::Text {
void GenerateCHeaderFile(std::shared_ptr<Streams::Stream> strm,
                         std::string name,
                         std::shared_ptr<TextStreams::TextWriter> writer);

std::string GenerateCHeaderFile(std::shared_ptr<Streams::Stream> strm,
                                std::string name);
void GenerateCHeaderFile(const std::vector<uint8_t> &data, std::string name,
                         std::shared_ptr<TextStreams::TextWriter> writer);
std::string GenerateCHeaderFile(const std::vector<uint8_t> &data,
                                std::string name);
} // namespace Tesses::Framework::Text