#include "TessesFramework/Streams/FileStream.hpp"
#if defined(_WIN32)
#include <windows.h>
#undef min
#endif
namespace Tesses::Framework::Streams
{
    void FileStream::SetMode(std::string mode)
    {
        this->canRead = false;
        this->canWrite = false;
        this->canSeek = true;
        if(mode.size() >= 1)
        {
            if(mode[0] == 'r') 
            {
                this->canRead = true;
            }
            else if(mode[0] == 'w')
            {
                this->canWrite = true;
            }
            else if(mode[0] == 'a')
            {
                this->canSeek = false;
                this->canWrite = true;
            }
        }

        if(((mode.size() >= 2  && mode[1] == '+') || (mode.size() >= 2 && mode[1] == 'b' && mode[2] == '+')))
        {
            this->canRead = true;
            this->canWrite = true;
        }
    }
    FileStream::FileStream(std::filesystem::path p, std::string mode)
    {
        std::string str = p.string();
        this->f = fopen(str.c_str(),mode.c_str());
        this->canSeek = true;
        this->owns=true;
        this->SetMode(mode);
    }
    FileStream::FileStream(FILE* f, bool owns, std::string mode , bool canSeek)
    {
        this->f = f;
        this->owns = owns;
        this->SetMode(mode);
        this->canSeek = canSeek;
    }
    size_t FileStream::Read(uint8_t* buff, size_t sz)
    {
        if(!CanRead()) throw std::runtime_error("Cannot read from stream");
        return fread(buff,1, sz, this->f);
    }
    size_t FileStream::Write(const uint8_t* buff, size_t sz)
    {
        if(!CanWrite()) throw std::runtime_error("Cannot write to stream");
        return fwrite(buff,1, sz, f);
    }
    bool FileStream::CanRead()
    {
        return this->canRead && this->f;
    }
    bool FileStream::CanWrite()
    {
        return this->canWrite && this->f;
    }
    bool FileStream::CanSeek()
    {
        return this->canSeek && this->f;
    }
    bool FileStream::EndOfStream()
    {
        if(!f) return true;
        return feof(this->f);
    }

    int64_t FileStream::GetPosition()
    {

        if(!f) return 0;
        #if defined(_WIN32)
        return (int64_t)_ftelli64(this->f);
        #else
        return (int64_t)ftello(this->f);
        #endif
    }
    void FileStream::Flush()
    {

        if(!f) return;
        fflush(this->f);
    }
    void FileStream::Seek(int64_t pos, SeekOrigin whence)
    {

        if(!f) return;
        #if defined(_WIN32)
        _fseeki64(this->f,pos,whence == SeekOrigin::Begin ? SEEK_SET : whence == SeekOrigin::Current ? SEEK_CUR : SEEK_END);
        #else
        fseeko(this->f,(off_t)pos,whence == SeekOrigin::Begin ? SEEK_SET : whence == SeekOrigin::Current ? SEEK_CUR : SEEK_END);
        #endif
    }
    FileStream::~FileStream()
    {   

        if(!f) return;
        if(this->owns)
             fclose(this->f);
    }
}