#pragma once
#include "TextReader.hpp"
#include "../Streams/Stream.hpp"
namespace Tesses::Framework::TextStreams
{
    class StreamReader : public TextReader
    {
        private:
            std::shared_ptr<Tesses::Framework::Streams::Stream> strm;
            bool owns;
        public:
            std::shared_ptr<Tesses::Framework::Streams::Stream> GetStream();
            StreamReader(std::shared_ptr<Tesses::Framework::Streams::Stream> strm);
            StreamReader(std::filesystem::path filename);
            bool ReadBlock(std::string& str,size_t sz);
            bool Rewind();
            ~StreamReader();
    };  
}   