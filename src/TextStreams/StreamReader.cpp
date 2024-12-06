#include "TessesFramework/TextStreams/StreamReader.hpp"
#include "TessesFramework/Streams/FileStream.hpp"
using Stream = Tesses::Framework::Streams::Stream;
using FileStream = Tesses::Framework::Streams::FileStream;

namespace Tesses::Framework::TextStreams {
    StreamReader::StreamReader(Stream& strm) : StreamReader(&strm, false)
    {

    }
    StreamReader::StreamReader(std::filesystem::path path) : StreamReader(new FileStream(path,"rb"),true)
    {

    }
    StreamReader::StreamReader(Stream* strm, bool owns) : TextReader()
    {
        this->strm = strm;
        this->owns = owns;
    }

    Stream& StreamReader::GetStream()
    {
        return *(this->strm);
    }

    bool StreamReader::ReadBlock(std::string& str, size_t len)
    {
        uint8_t buff[len];
        len = strm->ReadBlock(buff,len);
        if(len == 0) return false;
        str.append((const char*)buff, len);
        return true;
    }
    StreamReader::~StreamReader()
    {
        if(this->owns)
            delete this->strm;
    }
};