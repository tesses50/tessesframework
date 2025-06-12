#include "TextReader.hpp"


namespace Tesses::Framework::TextStreams
{
    class ConsoleReader : public TextReader {
        public:
            ConsoleReader();
            bool ReadBlock(std::string& str,size_t sz);
    };

    ConsoleReader StdIn();
}