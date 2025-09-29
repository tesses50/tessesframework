#pragma once
#include "../Streams/Stream.hpp"
#include "TextWriter.hpp"

namespace Tesses::Framework::TextStreams
{
    class StreamWriter : public TextWriter {
        private:
            std::shared_ptr<Tesses::Framework::Streams::Stream> strm;
        public:
            std::shared_ptr<Tesses::Framework::Streams::Stream> GetStream();
            StreamWriter(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
            StreamWriter(std::filesystem::path filename, bool append=false);
            void WriteData(const char* text, size_t len);
            ~StreamWriter();
    };
}