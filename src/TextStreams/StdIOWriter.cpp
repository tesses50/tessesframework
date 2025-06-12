#include "TessesFramework/TextStreams/StdIOWriter.hpp"
#if defined(__PS2__)
#include <debug.h>
#else
#include <cstdio>
#endif
namespace Tesses::Framework::TextStreams
{
    ConsoleWriter::ConsoleWriter(bool isError) : TextWriter()
    {
        this->isError=isError;
    }

    void ConsoleWriter::WriteData(const char* text, size_t len)
    {
        #if defined(__PS2__)
            char lenThing[10];//%.2048s
            while(len > 0) {
                int b = std::min((int)2047,(int)len);
                snprintf(lenThing,18,"%%.%is",b);
            
                scr_printf(lenThing,text);

                len -= b;
                text += b;
            }
        #else
            if(isError)
            fwrite(text,1,len,stderr);
            else
            fwrite(text,1,len,stdout);
        #endif
    }
    ConsoleWriter StdOut()
    {
        return ConsoleWriter(false);
    }
    ConsoleWriter StdErr()
    {
        return ConsoleWriter(true);
    }
}