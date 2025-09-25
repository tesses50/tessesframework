#pragma once
#include "../Streams/MemoryStream.hpp"
#include "../TextStreams/StringWriter.hpp"

namespace Tesses::Framework::Text {
    void GenerateCHeaderFile(Streams::Stream* strm,std::string name, TextStreams::TextWriter* writer);
   
    std::string GenerateCHeaderFile(Streams::Stream* strm,std::string name);
    void GenerateCHeaderFile(const std::vector<uint8_t>& data,std::string name, TextStreams::TextWriter* writer);
    std::string GenerateCHeaderFile(const std::vector<uint8_t>& data,std::string name);
}