#pragma once
#include "TextWriter.hpp"

namespace Tesses::Framework::TextStreams
{
    class StringWriter : public TextWriter {
        private:
            std::string text;
        public:
            std::string& GetString();
            StringWriter();
            StringWriter(std::string str);
            void WriteData(const char* text, size_t len);
    };
}
