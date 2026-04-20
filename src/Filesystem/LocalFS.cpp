#include "TessesFramework/Filesystem/LocalFS.hpp"
#include "TessesFramework/Streams/FileStream.hpp"
#include <cerrno>
#include <filesystem>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <windows.h>
#include "TessesFramework/Filesystem/VFSFix.hpp"
#undef min
#else
#include <utime.h>
#include <sys/statvfs.h>
#endif

#include "TessesFramework/Threading/Thread.hpp"
#if defined(__linux__)
#include <poll.h>
#include <sys/inotify.h>
#include <unistd.h>
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
    bool LocalFilesystem::Stat(VFSPath path, StatData& sfs)
    {
        std::string s = VFSPathToSystem(path);
        struct stat st;
        if(stat(s.c_str(),&st) == 0)
        {

            sfs.Device = (uint64_t)st.st_dev;
            sfs.Inode = (uint64_t)st.st_ino;
            sfs.Mode = (uint32_t)st.st_mode;
            sfs.HardLinks = (uint64_t)st.st_nlink;
            sfs.UserId = (uint32_t)st.st_uid;
            sfs.GroupId = (uint32_t)st.st_gid;
            sfs.DeviceId = (uint64_t)st.st_rdev;
            sfs.Size = (uint64_t)st.st_size;
            sfs.BlockSize = (uint64_t)st.st_blksize;
            sfs.BlockCount = (uint64_t)st.st_blocks;
            sfs.LastAccess = Date::DateTime((int64_t)st.st_atime);
            sfs.LastModified = Date::DateTime((int64_t)st.st_mtime);
            sfs.LastStatus = Date::DateTime((int64_t)st.st_ctime);
            return true;
        }
        return false;
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
        std::filesystem::path sysPath = VFSPathToSystem(path);
        if(!std::filesystem::is_directory(sysPath)) return VFSPathEnumerator();

        auto dir = new std::filesystem::directory_iterator(sysPath);
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
    void LocalFilesystem::Chown(VFSPath path, uint32_t uid, uint32_t gid)
    {
        auto pathStr = this->VFSPathToSystem(path);
        #if defined(_WIN32)

        #else
            chown(pathStr.c_str(), (uid_t)uid, (gid_t)gid);
        #endif
    }


    FIFOCreationResult LocalFilesystem::CreateFIFO(VFSPath path, uint32_t mod)
    {
        auto pathStr = this->VFSPathToSystem(path);
        #if defined(_WIN32)
            return FIFOCreationResult::Unsupported;
        #else
            int res = mkfifo(pathStr.c_str(), (mode_t)mod);
            if(res == 0) return FIFOCreationResult::Success;
            else if(res == -1)
            {
                switch(res)
                {
                    case EEXIST:
                        return FIFOCreationResult::Exists;
                    case ENOTSUP:
                        return FIFOCreationResult::Unsupported;
                    case EACCES:
                        return FIFOCreationResult::Denied;
                    case ENOSPC:
                        return FIFOCreationResult::OutOfInodes;
                    case EROFS:
                        return FIFOCreationResult::ReadOnlyFS;
                }
            }

            return FIFOCreationResult::UnknownError;
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
    #if defined(__linux__)

    class INotifyWatcher : public FSWatcher {
        std::shared_ptr<Threading::Thread> thrd;
        static uint32_t to_linux_mask(FSWatcherEventType flags)
        {
            uint32_t lflags = 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::Accessed) != 0) ? IN_ACCESS : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::AttributeChanged) != 0) ? IN_ATTRIB : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::Writen) != 0) ? IN_CLOSE_WRITE : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::Read) != 0) ? IN_CLOSE_NOWRITE : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::Created) != 0) ? IN_CREATE : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::Deleted) != 0) ? IN_DELETE : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::WatchEntryDeleted) != 0) ? IN_DELETE_SELF : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::Modified) != 0) ? IN_MODIFY : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::WatchEntryMoved) != 0) ? IN_MOVE_SELF : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::MoveOld) != 0) ? IN_MOVED_FROM : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::MoveNew) != 0) ? IN_MOVED_TO : 0;
            lflags |= (((uint32_t)flags & (uint32_t)FSWatcherEventType::Opened) != 0) ? IN_OPEN : 0;

            return lflags;
        }
        static FSWatcherEventType from_linux_mask(uint32_t lflags)
        {
            uint32_t flags = 0;
            flags |= ((lflags & IN_ACCESS) != 0) ? (uint32_t)FSWatcherEventType::Accessed : 0;
            flags |= ((lflags & IN_ATTRIB) != 0) ? (uint32_t)FSWatcherEventType::AttributeChanged : 0;
            flags |= ((lflags & IN_CLOSE_WRITE) != 0) ? (uint32_t)FSWatcherEventType::Writen : 0;
            flags |= ((lflags & IN_CLOSE_NOWRITE) != 0) ? (uint32_t)FSWatcherEventType::Read : 0;
            flags |= ((lflags & IN_CREATE) != 0) ? (uint32_t)FSWatcherEventType::Created : 0;
            flags |= ((lflags & IN_DELETE) != 0) ? (uint32_t)FSWatcherEventType::Deleted : 0;
            flags |= ((lflags & IN_DELETE_SELF) != 0) ? (uint32_t)FSWatcherEventType::WatchEntryDeleted : 0;
            flags |= ((lflags & IN_MODIFY) != 0) ? (uint32_t)FSWatcherEventType::Modified : 0;
            flags |= ((lflags & IN_MOVE_SELF) != 0) ? (uint32_t)FSWatcherEventType::WatchEntryMoved : 0;
            flags |= ((lflags & IN_MOVED_FROM) != 0) ? (uint32_t)FSWatcherEventType::MoveOld : 0;
            flags |= ((lflags & IN_MOVED_TO) != 0) ? (uint32_t)FSWatcherEventType::MoveNew : 0;
            flags |= ((lflags & IN_OPEN) != 0) ? (uint32_t)FSWatcherEventType::Opened : 0;

            return (FSWatcherEventType)flags;
        }
        public:
            INotifyWatcher(std::shared_ptr<VFS> vfs, VFSPath path) : FSWatcher(vfs,path)
            {

            }

        protected:

            void SetEnabledImpl(bool enabled)
            {
                if(enabled)
                {
                    int fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
                    if (fd == -1)
                    {
                        throw std::runtime_error("Cannot init inotify");
                    }
                    auto str =  this->GetFilesystem()->VFSPathToSystem(this->GetPath());

                    int watch = inotify_add_watch(fd, str.c_str(),to_linux_mask(this->events));

                    thrd = std::make_shared<Threading::Thread>([this,watch,fd]()-> void {
                        int cnt = 0;
                        struct pollfd pfd = {.fd = fd, .events = POLLIN};
                        std::vector<std::pair<VFSPath,uint32_t>> mvFroms;
                        char buf[4096]
                            __attribute__ ((aligned(__alignof__(struct inotify_event))));
                        const struct inotify_event *event;
                        ssize_t size;

                        bool fail=false;

                        FSWatcherEvent evt;
                        evt.dest = this->GetPath();
                        while(!fail && this->enabled)
                        {
                            cnt = poll(&pfd,1,-1);
                            if(cnt == -1) break;

                            if(cnt > 0)
                            {
                                if(pfd.revents & POLLIN)
                                {
                                    for (;;) {
                                        size = read(fd, buf, sizeof(buf));
                                        if (size == -1 && errno != EAGAIN) {
                                            fail=true;
                                            break;
                                        }

                                        if (size <= 0)
                                            break;

                                        for (char *ptr = buf; ptr < buf + size;
                                                                ptr += sizeof(struct inotify_event) + event->len) {

                                            event = (const struct inotify_event *) ptr;
                                            VFSPath path = this->GetPath();

                                            if(event->len)
                                                path = path / std::string(event->name, (size_t)event->len);

                                            if(((uint32_t)this->events & (uint32_t)FSWatcherEventType::Moved) == (uint32_t)FSWatcherEventType::Moved && event->mask & IN_MOVED_FROM)
                                            {
                                                mvFroms.emplace_back(path,event->cookie);
                                            }
                                            else if(((uint32_t)this->events & (uint32_t)FSWatcherEventType::Moved) == (uint32_t)FSWatcherEventType::Moved && event->mask & IN_MOVED_TO)
                                            {
                                                for(auto ittr = mvFroms.begin(); ittr != mvFroms.end(); ittr++)
                                                {
                                                    if(ittr->second == event->cookie)
                                                    {
                                                        evt.src = ittr->first;
                                                        mvFroms.erase(ittr);
                                                        break;
                                                    }
                                                }
                                                evt.isDir = (event->mask & IN_ISDIR);
                                                evt.dest = path;
                                                evt.type = FSWatcherEventType::Moved;
                                                if(this->event)
                                                    this->event(evt);
                                            }
                                            else {

                                                evt.isDir = (event->mask & IN_ISDIR);
                                                evt.src = path;
                                                evt.type = from_linux_mask(event->mask);;
                                                if(this->event)
                                                    this->event(evt);
                                            }
                                            if(event->mask & IN_MOVE_SELF)
                                            {
                                                close(fd);
                                                return;
                                            }
                                            if(event->mask & IN_DELETE_SELF)
                                            {
                                                close(fd);
                                                return;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        close(fd);
                    });


                }
                else
                {
                    thrd = nullptr;
                }
            }
            public:
            ~INotifyWatcher()
            {
                this->enabled = false;
            }
    };
    #endif



    std::shared_ptr<FSWatcher> LocalFilesystem::CreateWatcher(std::shared_ptr<VFS> vfs, VFSPath path)
    {
        #if defined(__linux__)
            return std::make_shared<INotifyWatcher>(vfs, path);
        #endif
        return VFS::CreateWatcher(vfs,path);
    }

    std::shared_ptr<LocalFilesystem> LocalFS = std::make_shared<LocalFilesystem>();


}

// C:/Users/Jim/Joel
