#include "TextWriter.hpp"


namespace Tesses::Framework::TextStreams
{
    class ConsoleWriter : public TextWriter {
        bool isError;
        public:
            ConsoleWriter(bool isError=false);
            void WriteData(const char* text, size_t len);
    };

    ConsoleWriter StdOut();
    ConsoleWriter StdErr();
}