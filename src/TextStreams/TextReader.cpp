#include "TessesFramework/TextStreams/TextReader.hpp"

namespace Tesses::Framework::TextStreams
{
    int32_t TextReader::ReadChar()
    {
        std::string txt;
        this->ReadBlock(txt,1);
        if(txt.empty()) return -1;
        return (uint8_t)txt[0];
    }
    std::string TextReader::ReadLine()
    {
        std::string str = {};
        ReadLine(str);
        return str;
    }
    bool TextReader::ReadLine(std::string& str)
    {
        bool ret = false;
        int32_t r = -1;
        do {
            r = ReadChar();
            if(r == -1)  break;
            if(r == '\r') continue;
            if(r == '\n') break;
            str.push_back((char)(uint8_t)r);
            ret = true;
        } while(r != -1);
        return ret;
    }

    std::string TextReader::ReadToEnd()
    {
        std::string str = {};
        ReadToEnd(str);
        return str;
    }

    void TextReader::ReadToEnd(std::string& str)
    {
        while(ReadBlock(str,1024));
    }
    void TextReader::CopyTo(TextWriter& writer, size_t buffSz)
    {
        std::string str = {};
        while(ReadBlock(str,buffSz))
        {
            writer.Write(str);
            str.clear();
        }
    }

    TextReader::~TextReader()
    {
        
    }
}   