#include "TessesFramework/TextStreams/StringReader.hpp"

namespace Tesses::Framework::TextStreams {
    StringReader::StringReader()
    {
        this->offset=0;
        this->str="";
    }
    StringReader::StringReader(std::string str)
    {
        this->offset=0;
        this->str=str;
    }
    size_t& StringReader::GetOffset()
    {
        return this->offset;
    }
    std::string& StringReader::GetString()
    {
        return this->str;
    }
    bool StringReader::Rewind()
    {
        this->offset=0;
        return true;
    }
    bool StringReader::ReadBlock(std::string& str,size_t sz)
    {
        if(this->offset < this->str.size())
        {
            size_t len = std::min(sz,this->str.size()-this->offset);
            str.insert(str.size(),this->str.data()+this->offset,len);
            offset+=len;
            return true;
        }
        return false;
    }
}