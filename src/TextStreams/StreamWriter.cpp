#include "TessesFramework/Streams/FileStream.hpp"
#include "TessesFramework/TextStreams/StreamWriter.hpp"
using Stream = Tesses::Framework::Streams::Stream;
using FileStream = Tesses::Framework::Streams::FileStream;

namespace Tesses::Framework::TextStreams
{
    Stream& StreamWriter::GetStream()
    {
        return *(this->strm);
    }
    StreamWriter::StreamWriter(Stream& strm) : StreamWriter(&strm, false)
    {

    }
    StreamWriter::StreamWriter(Stream* strm, bool owns) : TextWriter()
    {
        this->strm = strm;
        this->owns = owns;
    }
    StreamWriter::StreamWriter(std::filesystem::path filename, bool append) : StreamWriter(new FileStream(filename, append ? "ab" : "wb"),true)
    {

    }
    void StreamWriter::WriteData(const char* text, size_t len)
    {
        this->strm->WriteBlock((const uint8_t*)text, len);
    }
    StreamWriter::~StreamWriter()
    {
        if(this->owns)
            delete this->strm;
    }
}