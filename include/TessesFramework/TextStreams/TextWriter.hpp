#pragma once
#include "../Common.hpp"

namespace Tesses::Framework::TextStreams
{
    class TextWriter {
        public:
            TextWriter();
            std::string newline;
            virtual void WriteData(const char* text, size_t len)=0;
            void Write(std::string txt);
            void WriteLine(std::string txt);
            void WriteLine();
            virtual ~TextWriter();
    };
}