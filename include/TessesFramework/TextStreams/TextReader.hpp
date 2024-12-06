#pragma once
#include "../Common.hpp"
#include "TextWriter.hpp"
namespace Tesses::Framework::TextStreams
{
    class TextReader 
    {
        public:
            virtual bool ReadBlock(std::string& str,size_t sz)=0;
            int32_t ReadChar();
            std::string ReadLine();
            bool ReadLine(std::string& str);
            std::string ReadToEnd();
            void ReadToEnd(std::string& str);
            void CopyTo(TextWriter& writer, size_t bufSz=1024);
            virtual ~TextReader();
    };
}