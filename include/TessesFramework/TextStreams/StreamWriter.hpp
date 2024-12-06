#pragma once
#include "../Streams/Stream.hpp"
#include "TextWriter.hpp"

namespace Tesses::Framework::TextStreams
{
    class StreamWriter : public TextWriter {
        private:
            Tesses::Framework::Streams::Stream* strm;
            bool owns;
        public:
            Tesses::Framework::Streams::Stream& GetStream();
            StreamWriter(Tesses::Framework::Streams::Stream& strm);
            StreamWriter(Tesses::Framework::Streams::Stream* strm, bool owns);
            StreamWriter(std::filesystem::path filename, bool append=false);
            void WriteData(const char* text, size_t len);
            ~StreamWriter();
    };
}