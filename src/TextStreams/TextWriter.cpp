#include "TessesFramework/TextStreams/TextWriter.hpp"

namespace Tesses::Framework::TextStreams
{
    TextWriter::TextWriter()
    {
        #if defined(WIN32) || defined(_WIN32)
        newline = "\r\n";
        #else
        newline = "\n";
        #endif
    }
    void TextWriter::Write(std::string txt)
    {
        WriteData(txt.c_str(),txt.size());
    }
    void TextWriter::WriteLine(std::string txt)
    {
        std::string str = txt;
        str.append(newline);
        Write(str);
    }
    void TextWriter::WriteLine()
    {
        Write(newline);
    }
    TextWriter::~TextWriter()
    {

    }
}