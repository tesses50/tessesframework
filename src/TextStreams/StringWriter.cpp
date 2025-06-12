#include "TessesFramework/TextStreams/StringWriter.hpp"

namespace Tesses::Framework::TextStreams
{
    std::string& StringWriter::GetString()
    {
        return this->text;
    }
    StringWriter::StringWriter() : TextWriter()
    {

    }
    StringWriter::StringWriter(std::string str) : TextWriter()
    {
        this->text = str;
    }
    void StringWriter::WriteData(const char* text, size_t len)
    {
        this->text.append(text,len);
    }
}