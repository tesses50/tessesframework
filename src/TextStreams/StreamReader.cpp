#include "TessesFramework/TextStreams/StreamReader.hpp"
#include "TessesFramework/Streams/FileStream.hpp"
using Stream = Tesses::Framework::Streams::Stream;
using FileStream = Tesses::Framework::Streams::FileStream;

namespace Tesses::Framework::TextStreams {
   
    StreamReader::StreamReader(std::filesystem::path path) : StreamReader(std::make_shared<FileStream>(path,"rb"))
    {

    }
    bool StreamReader::Rewind()
    {
        if(this->strm->CanSeek())
        {
            this->strm->Seek((int64_t)0,Tesses::Framework::Streams::SeekOrigin::Begin);
            return true;
        }
        return false;
    }
    StreamReader::StreamReader(std::shared_ptr<Stream> strm) : TextReader()
    {
        this->strm = strm;
    }

    std::shared_ptr<Stream> StreamReader::GetStream()
    {
        return (this->strm);
    }

    bool StreamReader::ReadBlock(std::string& str, size_t len)
    {
        std::vector<uint8_t> buff(len);
       
        len = strm->ReadBlock(buff.data(),len);
        if(len == 0) { return false;}
        str.append((const char*)buff.data(), len);
        
    
        return true;
    }
    StreamReader::~StreamReader()
    {
        
    }
};