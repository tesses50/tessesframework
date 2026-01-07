#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/Streams/FileStream.hpp"
#include <iostream>
#include <sys/stat.h>
#if defined(_WIN32)
#include <windows.h>
#include "TessesFramework/Filesystem/VFSFix.hpp"
#undef min
#else
#include <utime.h>
#include <sys/statvfs.h>
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
    void LocalFilesystem::GetDate(VFSPath path, Date::DateTime& lastWrite, Date::DateTime& lastAccess)
    {
        std::string s = VFSPathToSystem(path);
        struct stat st;
        if(stat(s.c_str(),&st) == 0)
        {
            lastAccess = Date::DateTime((int64_t)st.st_atime);
            lastWrite = Date::DateTime((int64_t)st.st_mtime);
        }
    }
    void LocalFilesystem::SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess)
    {
        std::string s = VFSPathToSystem(path);
        #if defined(TESSESFRAMEWORK_ENABLE_SETDATE)
	#if defined(_WIN32)
        FILETIME lastWriteF;
        FILETIME lastAccessF;
        
        TimetToFileTime((time_t)lastWrite.ToEpoch(),&lastWriteF);
        TimetToFileTime((time_t)lastAccess.ToEpoch(),&lastAccessF);
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
        utim.actime = (time_t)lastAccess.ToEpoch();
        utim.modtime = (time_t)lastWrite.ToEpoch();
        utime(s.c_str(),&utim);
        #endif
	#endif
    }
    VFSPath LocalFilesystem::ReadLink(VFSPath path)
    {
        auto res = std::filesystem::read_symlink(this->VFSPathToSystem(path)).string();
        return this->SystemToVFSPath(res.c_str());
    }
    std::shared_ptr<Tesses::Framework::Streams::Stream> LocalFilesystem::OpenFile(VFSPath path, std::string mode)
    {
        return std::make_shared<Tesses::Framework::Streams::FileStream>(VFSPathToSystem(path), mode);
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

                if(!firstPartPath.empty() && firstPartPath.back() == ':') p.relative=false;
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
    bool LocalFilesystem::StatVFS(VFSPath path, StatVFSData& data)
    {
        auto pathStr = this->VFSPathToSystem(path);
        #if defined(_WIN32)
        //not supporting windows yet
        VFS::StatVFS(path, data);
        return true;
        #else
        struct statvfs vfs;
        if(statvfs(pathStr.c_str(), &vfs) == 0)
        {
            data.BlockSize = vfs.f_bsize;
            data.FragmentSize = vfs.f_frsize;
            data.Blocks = vfs.f_blocks;
            data.BlocksFree = vfs.f_bfree;
            data.BlocksAvailable = vfs.f_bavail;
            data.TotalInodes = vfs.f_files;
            data.FreeInodes = vfs.f_ffree;
            data.AvailableInodes = vfs.f_favail;
            data.Id = vfs.f_fsid;
            data.Flags = vfs.f_flag;
            data.MaxNameLength = vfs.f_namemax;
            return true;
        }
        return false;
        #endif
    }

    void LocalFilesystem::Chmod(VFSPath path, uint32_t mode)
    {
        auto pathStr = this->VFSPathToSystem(path);
        #if defined(_WIN32)

        #else
            chmod(pathStr.c_str(), (mode_t)mode);
        #endif
    }

    
    void LocalFilesystem::Lock(VFSPath path)
    {
        auto p2 = VFSPathToSystem(path);
        const char* fopenPath = p2.c_str();
        while(true)
        {
            FILE* fp = fopen(fopenPath,"wx");
            if(fp) {
                fclose(fp);
                break;
            }
        }
    }
    void LocalFilesystem::Unlock(VFSPath path)
    {
        std::error_code error;
        std::filesystem::remove(VFSPathToSystem(path),error);
    }

    std::shared_ptr<LocalFilesystem> LocalFS = std::make_shared<LocalFilesystem>();
}

// C:/Users/Jim/Joel
