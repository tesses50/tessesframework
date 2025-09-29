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
        uint8_t* buff = new uint8_t[len];
       
        len = strm->ReadBlock(buff,len);
        if(len == 0) {delete buff; return false;}
        str.append((const char*)buff, len);
        
        delete buff;
    
        return true;
    }
    StreamReader::~StreamReader()
    {
        
    }
};