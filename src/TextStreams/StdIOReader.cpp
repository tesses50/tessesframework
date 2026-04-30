#include "TessesFramework/TextStreams/StdIOReader.hpp"


namespace Tesses::Framework::TextStreams
{
    ConsoleReader::ConsoleReader()
    {

    }
    bool ConsoleReader::ReadBlock(std::string& str,size_t len)
    {
        std::vector<uint8_t> buff(len);
        
        size_t read=0;
        size_t readTotal=0;
        uint8_t* buffOff=buff.data();
        do {    
            read=fread(buffOff,1,len,stdin);
            if(read != 0) {readTotal+= read;len-=read; buffOff+=read;}
        } while(read != 0);
        if(readTotal == 0) return false;
        str.append((const char*)buff.data(), readTotal);
        
        return true;
    }

    ConsoleReader StdIn()
    {
        return ConsoleReader();
    }
}