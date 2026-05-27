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

#include "TessesFramework/Text/HeaderGenerator.hpp"
namespace Tesses::Framework::Text {

void GenerateCHeaderFile(std::shared_ptr<Streams::Stream> strm,
                         std::string name,
                         std::shared_ptr<TextStreams::TextWriter> writer) {
    const size_t BLK_SZ = 1024;
    writer->WriteLine("#pragma once");
    writer->WriteLine("#if defined(__cplusplus)");
    writer->WriteLine("extern \"C\" {");
    writer->WriteLine("#endif");
    writer->WriteLine("#include <stdint.h>");
    writer->WriteLine("#include <stddef.h>");
    writer->Write("const uint8_t ");
    writer->Write(name);
    writer->WriteLine("_data[] = {");
    uint64_t total = 0;
    size_t read;

    std::vector<uint8_t> data(BLK_SZ);
    bool first = true;

    do {
        read = strm->ReadBlock(data.data(), data.size());

        for (size_t i = 0; i < read; i++) {
            if (!first)
                writer->Write(", ");
            writer->Write((uint64_t)data[i]);
            first = false;
        }
        total += read;
    } while (read != 0);

    writer->WriteLine("};");
    writer->Write("const size_t ");
    writer->Write(name);
    writer->Write("_length = ");
    writer->Write(total);
    writer->WriteLine(";");

    writer->WriteLine("#if defined(__cplusplus)");
    writer->WriteLine("}");
    writer->WriteLine("#endif");
}

std::string GenerateCHeaderFile(std::shared_ptr<Streams::Stream> strm,
                                std::string name) {
    auto writer = std::make_shared<TextStreams::StringWriter>();
    GenerateCHeaderFile(strm, name, writer);
    return writer->GetString();
}
void GenerateCHeaderFile(const std::vector<uint8_t> &data, std::string name,
                         std::shared_ptr<TextStreams::TextWriter> writer) {
    auto ms = std::make_shared<Tesses::Framework::Streams::MemoryStream>(false);
    ms->GetBuffer() = data;
    GenerateCHeaderFile(ms, name, writer);
}
std::string GenerateCHeaderFile(const std::vector<uint8_t> &data,
                                std::string name) {
    auto writer = std::make_shared<TextStreams::StringWriter>();
    GenerateCHeaderFile(data, name, writer);
    return writer->GetString();
}
}; // namespace Tesses::Framework::Text