#pragma once
#include "../Streams/MemoryStream.hpp"
#include "../TextStreams/StringWriter.hpp"

namespace Tesses::Framework::Text {
    void GenerateCHeaderFile(std::shared_ptr<Streams::Stream> strm,std::string name, std::shared_ptr<TextStreams::TextWriter> writer);
   
    std::string GenerateCHeaderFile(std::shared_ptr<Streams::Stream> strm,std::string name);
    void GenerateCHeaderFile(const std::vector<uint8_t>& data,std::string name, std::shared_ptr<TextStreams::TextWriter> writer);
    std::string GenerateCHeaderFile(const std::vector<uint8_t>& data,std::string name);
}