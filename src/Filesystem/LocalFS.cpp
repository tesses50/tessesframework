#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/Streams/FileStream.hpp"
#include <iostream>
#include <sys/stat.h>
#include <utime.h>
namespace Tesses::Framework::Filesystem
{
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
        struct utimbuf utim;
        utim.actime = lastAccess;
        utim.modtime = lastWrite;
        utime(s.c_str(),&utim);
    }
    VFSPath LocalFilesystem::ReadLink(VFSPath path)
    {
        return this->SystemToVFSPath(std::filesystem::read_symlink(this->VFSPathToSystem(path)));
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
        #if defined(WIN32)
        bool first=true;
        std::string p = {};
        for(auto item : path.path)
        {
            if(!(first && !item.empty() && item.back()==':') && !(first && this->relative))
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
        p.path = VFSPath::SplitPath(path,true);
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