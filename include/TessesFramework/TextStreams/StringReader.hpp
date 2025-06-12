#include "TextReader.hpp"

namespace Tesses::Framework::TextStreams {
    class StringReader : public TextReader {
        std::string str;
        size_t offset;
        public:
            StringReader();
            StringReader(std::string str);
            size_t& GetOffset();
            std::string& GetString();
            bool Rewind();
            bool ReadBlock(std::string& str,size_t sz);
    };
}