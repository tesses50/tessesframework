#pragma once
#include "Stream.hpp"
namespace Tesses::Framework::Streams
{
    struct WindowSize {
        uint16_t Width;
        uint16_t Height;
        uint16_t Columns;
        uint16_t Rows;
    };
    class PtyStream : public Stream
    {
        int socket;
        int64_t pid;
        bool eos;
        WindowSize wS;
        
        public:
            PtyStream(WindowSize sz,std::string filename, std::vector<std::string> args,std::vector<std::string> env);
            bool EndOfStream();
            bool CanRead();
            bool CanWrite();
            size_t Read(uint8_t* buff, size_t sz);
            size_t Write(const uint8_t* buff, size_t sz);
            void Resize(WindowSize sz);
            WindowSize GetWindowSize();
            ~PtyStream();
    };
}