#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/Streams/FileStream.hpp"
#include <iostream>
#include <sys/stat.h>
#if defined(_WIN32)
#include <windows.h>
#undef min
#else
#include <utime.h>
#endif
namespace Tesses::Framework::Filesystem
{
    #if defined(_WIN32)
    static void TimetToFileTime(time_t t, LPFILETIME pft) {
        ULARGE_INTEGER time_value;
        time_value.QuadPart = (t * 10000000LL) + 116444736000000000LL;
        pft->dwLowDateTime = time_value.LowPart;
        pft->dwHighDateTime = time_value.HighPart;
    }
    #endif
    void LocalFilesystem::GetDate(VFSPath path, time_t& lastWrite, time_t& lastAccess)
    {
        std::string s = VFSPathToSystem(path);
        struct stat st;
        if(stat(s.c_str(),&st) == 0)
        {
            lastAccess = st.st_atime;
            lastWrite = st.st_mtime;
        }
    }
    void LocalFilesystem::SetDate(VFSPath path, time_t lastWrite, time_t lastAccess)
    {
        std::string s = VFSPathToSystem(path);
        #if defined(_WIN32)
        FILETIME lastWriteF;
        FILETIME lastAccessF;
        TimetToFileTime(lastWrite,&lastWriteF);
        TimetToFileTime(lastAccess,&lastAccessF);
        HANDLE hFile = CreateFileA(
            s.c_str(),
            FILE_WRITE_ATTRIBUTES,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS, // For directories
            NULL
        );
        if(hFile != INVALID_HANDLE_VALUE)
        {
            SetFileTime(
                hFile,
                NULL,
                &lastAccessF,
                &lastWriteF
            );
            CloseHandle(hFile);
        }
        #else
        struct utimbuf utim;
        utim.actime = lastAccess;
        utim.modtime = lastWrite;
        utime(s.c_str(),&utim);
        #endif
    }
    VFSPath LocalFilesystem::ReadLink(VFSPath path)
    {
        auto res = std::filesystem::read_symlink(this->VFSPathToSystem(path)).string();
        return this->SystemToVFSPath(res.c_str());
    }
    Tesses::Framework::Streams::Stream* LocalFilesystem::OpenFile(VFSPath path, std::string mode)
    {
        return new Tesses::Framework::Streams::FileStream(VFSPathToSystem(path), mode);
    }

    void LocalFilesystem::DeleteDirectory(VFSPath path)
    {
        std::filesystem::remove(VFSPathToSystem(path));
    }
    void LocalFilesystem::DeleteFile(VFSPath path)
    {
        std::filesystem::remove(VFSPathToSystem(path));
    }
    void LocalFilesystem::CreateDirectory(VFSPath path)
    {
        std::filesystem::create_directories(VFSPathToSystem(path));
    }
    bool LocalFilesystem::DirectoryExists(VFSPath path)
    {
        return std::filesystem::is_directory(VFSPathToSystem(path));
    }
    bool LocalFilesystem::RegularFileExists(VFSPath path)
    {
        return std::filesystem::is_regular_file(VFSPathToSystem(path));
    }
    bool LocalFilesystem::SymlinkExists(VFSPath path)
    {
        return std::filesystem::is_symlink(VFSPathToSystem(path));
    }
    bool LocalFilesystem::BlockDeviceExists(VFSPath path)
    {
        return std::filesystem::is_block_file(VFSPathToSystem(path));
    }
    bool LocalFilesystem::CharacterDeviceExists(VFSPath path)
    {
        return std::filesystem::is_character_file(VFSPathToSystem(path));
    }
    bool LocalFilesystem::SocketFileExists(VFSPath path)
    {
        return std::filesystem::is_socket(VFSPathToSystem(path));
    }
    bool LocalFilesystem::FIFOFileExists(VFSPath path)
    {
        return std::filesystem::is_fifo(VFSPathToSystem(path));
    }
    void LocalFilesystem::CreateSymlink(VFSPath existingFile, VFSPath symlinkFile)
    {
        if(std::filesystem::is_directory(VFSPathToSystem(existingFile)))
        {
            std::filesystem::create_directory_symlink(VFSPathToSystem(existingFile),VFSPathToSystem(symlinkFile));
        }
        else
        {
            std::filesystem::create_symlink(VFSPathToSystem(existingFile),VFSPathToSystem(symlinkFile));
        }
    }
    void LocalFilesystem::CreateHardlink(VFSPath existingFile, VFSPath newName)
    {
        std::filesystem::create_hard_link(VFSPathToSystem(existingFile),VFSPathToSystem(newName));
    }
    void LocalFilesystem::MoveFile(VFSPath src, VFSPath dest)
    {
        std::filesystem::rename(VFSPathToSystem(src),VFSPathToSystem(dest));
    }
    void LocalFilesystem::MoveDirectory(VFSPath src, VFSPath dest)
    {
        std::filesystem::rename(VFSPathToSystem(src),VFSPathToSystem(dest));
    }
    std::string LocalFilesystem::VFSPathToSystem(VFSPath path)
    {
        #if defined(_WIN32)
        bool first=true;
        std::string p = {};
        for(auto item : path.path)
        {
            if(!(first && !item.empty() && item.back()==':') && !(first && path.relative))
                p.push_back('\\');
            p.append(item);
            first=false;
        }
        return p;

        #else
        return path.ToString();
        #endif
    }
    VFSPath LocalFilesystem::SystemToVFSPath(std::string path)
    {
        VFSPath p;
        p.path = VFSPath::SplitPath(path);
        p.relative=true;
        if(!path.empty())
        {
            if(path.front() == '/') p.relative=false;
            if(!p.path.empty())
            {
                auto firstPartPath = p.path.front();

                if(!firstPartPath.empty() && firstPartPath.back() == '/') p.relative=false;
            }
        }
        return p;
    }
    
    VFSPathEnumerator LocalFilesystem::EnumeratePaths(VFSPath path)
    {
        auto dir = new std::filesystem::directory_iterator(VFSPathToSystem(path));
        return VFSPathEnumerator([dir,path](VFSPath& path0)->bool {
            std::filesystem::directory_iterator& ittr = *dir;
            if(ittr != std::filesystem::directory_iterator())
            {
                path0 = VFSPath(path, ittr->path().filename().string());
                ittr++;
                return true;
            }
            return false;
        },[dir]()->void{
            delete dir;
        });
    }
}

// C:/Users/Jim/Joel