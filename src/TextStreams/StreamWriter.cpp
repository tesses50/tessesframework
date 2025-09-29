#include "TessesFramework/Streams/FileStream.hpp"
#include "TessesFramework/TextStreams/StreamWriter.hpp"
using Stream = Tesses::Framework::Streams::Stream;
using FileStream = Tesses::Framework::Streams::FileStream;

namespace Tesses::Framework::TextStreams
{
    std::shared_ptr<Stream> StreamWriter::GetStream()
    {
        return this->strm;
    }
    StreamWriter::StreamWriter(std::shared_ptr<Stream> strm) : TextWriter()
    {

        this->strm = strm;
    }
    StreamWriter::StreamWriter(std::filesystem::path filename, bool append) : StreamWriter(std::make_shared<FileStream>(filename, append ? "ab" : "wb"))
    {

    }
    void StreamWriter::WriteData(const char* text, size_t len)
    {
        this->strm->WriteBlock((const uint8_t*)text, len);
    }
    StreamWriter::~StreamWriter()
    {
    }
}