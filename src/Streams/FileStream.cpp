#include "TessesFramework/Streams/FileStream.hpp"

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
        this->f = fopen(p.c_str(),mode.c_str());
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
        return fread(buff,1, sz, this->f);
    }
    size_t FileStream::Write(const uint8_t* buff, size_t sz)
    {
        return fwrite(buff,1, sz, f);
    }
    bool FileStream::CanRead()
    {
        return this->canRead;
    }
    bool FileStream::CanWrite()
    {
        return this->canWrite;
    }
    bool FileStream::CanSeek()
    {
        return this->canSeek;
    }
    int64_t FileStream::GetPosition()
    {
        return (int64_t)ftello(this->f);
    }
    void FileStream::Flush()
    {
        fflush(this->f);
    }
    void FileStream::Seek(int64_t pos, SeekOrigin whence)
    {
        fseeko(this->f,(off_t)pos,whence == SeekOrigin::Begin ? SEEK_SET : whence == SeekOrigin::Current ? SEEK_CUR : SEEK_END);
    }
    FileStream::~FileStream()
    {   
        if(this->owns)
             fclose(this->f);
    }
}