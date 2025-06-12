#pragma once
#include "TextReader.hpp"
#include "../Streams/Stream.hpp"
namespace Tesses::Framework::TextStreams
{
    class StreamReader : public TextReader
    {
        private:
            Tesses::Framework::Streams::Stream* strm;
            bool owns;
        public:
            Tesses::Framework::Streams::Stream& GetStream();
            StreamReader(Tesses::Framework::Streams::Stream& strm);
            StreamReader(Tesses::Framework::Streams::Stream* strm, bool owns);
            StreamReader(std::filesystem::path filename);
            bool ReadBlock(std::string& str,size_t sz);
            bool Rewind();
            ~StreamReader();
    };  
}   