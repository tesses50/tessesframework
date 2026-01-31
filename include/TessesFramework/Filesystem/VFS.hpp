#pragma once
#include "../Common.hpp"
#include "../Streams/Stream.hpp"
#include <functional>
#include <memory>
#include "../Date/Date.hpp"
#include "VFSFix.hpp"
            
namespace Tesses::Framework::Filesystem
{

    class StatVFSData {
        public:
            uint64_t BlockSize;
            uint64_t FragmentSize;
            uint64_t Blocks;
            uint64_t BlocksFree;
            uint64_t BlocksAvailable;
            uint64_t TotalInodes;
            uint64_t FreeInodes;
            uint64_t AvailableInodes;
            uint64_t Id;
            uint64_t Flags;
            uint64_t MaxNameLength;
    };
    class VFSPath {
        public:
            static VFSPath CurrentDirectoryAsRelative();
            bool relative;
            static std::vector<std::string> SplitPath(std::string path);
            std::vector<std::string> path;  
            VFSPath();
            explicit VFSPath(const char* path) : VFSPath(std::string(path))
            {}
            VFSPath(std::vector<std::string> path);
            VFSPath(std::string path);
            VFSPath(VFSPath p, std::string subent);
            VFSPath(VFSPath p, VFSPath p2);
            
            //does not check for ?
            static VFSPath ParseUriPath(std::string path);

            VFSPath GetParent() const;
            VFSPath CollapseRelativeParents() const;
            std::string GetFileName() const;
            bool HasExtension() const;
            std::string GetExtension() const;
            void ChangeExtension(std::string ext);
            void RemoveExtension();
            std::string ToString() const;

            static VFSPath GetAbsoluteCurrentDirectory();
            static void SetAbsoluteCurrentDirectory(VFSPath path);
            VFSPath MakeAbsolute() const;
    
            VFSPath MakeAbsolute(VFSPath curDir) const;
            VFSPath MakeRelative() const;
            VFSPath MakeRelative(VFSPath toMakeRelativeTo) const;
    };
    VFSPath operator/(VFSPath p, VFSPath p2);
    VFSPath operator/(VFSPath p, std::string p2);
    VFSPath operator/(std::string p, VFSPath p2);
    VFSPath operator+(VFSPath p, VFSPath p2);
    VFSPath operator+(VFSPath p, std::string p2);
    VFSPath operator+(std::string p, VFSPath p2);
    bool operator==(VFSPath p,VFSPath p2);
    bool operator!=(VFSPath p,VFSPath p2);
    bool operator==(std::string p,VFSPath p2);
    bool operator!=(std::string p,VFSPath p2);
    bool operator==(VFSPath p,std::string p2);
    bool operator!=(VFSPath p,std::string p2);
    class VFSPathEnumeratorData {
        public:
            VFSPathEnumeratorData(std::function<bool(VFSPath&)> moveNext, std::function<void()> destroy)
            {
                this->eof=false;
                this->moveNext=moveNext;
                this->destroy=destroy;
            }
            bool eof;
            std::function<bool(VFSPath&)> moveNext;
            std::function<void()> destroy;
            ~VFSPathEnumeratorData()
            {
                this->destroy();
            }
    };

    class VFSPathEnumerator;

    class VFSPathEnumeratorItterator
    {
        VFSPath e;
        VFSPathEnumerator* enumerator;
        public:
            VFSPathEnumeratorItterator();
            VFSPathEnumeratorItterator(VFSPathEnumerator* enumerator);

            VFSPathEnumeratorItterator& operator++();
            VFSPathEnumeratorItterator& operator++(int);
            
            VFSPath& operator*();
            VFSPath* operator->();
            
            bool operator!=(VFSPathEnumeratorItterator right);
            bool operator==(VFSPathEnumeratorItterator right);
    };

    class VFSPathEnumerator {
        std::shared_ptr<VFSPathEnumeratorData> data;
        public:
            VFSPathEnumerator();
            VFSPathEnumerator* MakePointer();
            VFSPathEnumerator(std::function<bool(VFSPath&)> moveNext,  std::function<void()> destroy);
            VFSPath Current;
            bool MoveNext();
            bool IsDone();

            VFSPathEnumeratorItterator begin();

            VFSPathEnumeratorItterator end();
    };
    enum class FSWatcherEventType {

        None = 0,
        //IN_ACCESS
        Accessed=1,
        //IN_ATTRIB
        AttributeChanged =2,
        //IN_CLOSE_WRITE
        Writen = 4,
        //IN_CLOSE_NOWRITE
        Read = 8,
        //IN_CREATE
        Created = 16,
        //IN_DELETE
        Deleted = 32,
        //IN_DELETE_SELF
        WatchEntryDeleted = 64,
        //IN_MODIFY
        Modified = 128,
        //IN_MOVE_SELF
        WatchEntryMoved = 256,
        //IN_MOVED_FROM
        MoveOld = 512,
        //IN_MOVED_TO
        MoveNew = 1024,
        //IN_OPEN
        Opened = 2048,
        //IN_CLOSE
        Closed = Writen | Read,
        //IN_MOVE
        Moved = MoveOld | MoveNew,
        //IN_ALL_EVENTS
        All = Accessed | AttributeChanged | Created | Deleted | WatchEntryDeleted | Modified | WatchEntryMoved | Opened | Closed | Moved
    };
    struct FSWatcherEvent {
        //the file or source on move
        VFSPath src;
        //the dest when moving
        VFSPath dest;
        FSWatcherEventType type;
        bool isDir;

        bool IsEvent(FSWatcherEventType e);

        std::string ToString();
    };
    class VFS;

    class FSWatcher {
        private:
            std::shared_ptr<VFS> vfs;
            VFSPath path;
        protected:

            std::atomic<bool> enabled=false;
            virtual void SetEnabledImpl(bool enabled);
        public:
            FSWatcher(std::shared_ptr<VFS> vfs, VFSPath path);
            std::function<void(FSWatcherEvent&)> event;
            FSWatcherEventType events = FSWatcherEventType::All;
            bool GetEnabled();
            void SetEnabled(bool val);
            std::shared_ptr<VFS> GetFilesystem();
            const VFSPath& GetPath();
            virtual ~FSWatcher() = default;
            
            static std::shared_ptr<FSWatcher> Create(std::shared_ptr<VFS> vfs, VFSPath path); 
    };

    class VFS {
        public:
            
            virtual std::shared_ptr<Tesses::Framework::Streams::Stream> OpenFile(VFSPath path, std::string mode)=0;
            virtual void CreateDirectory(VFSPath path)=0;
            virtual void DeleteDirectory(VFSPath path)=0;
            virtual bool RegularFileExists(VFSPath path)=0;
            virtual bool SymlinkExists(VFSPath path);
            virtual bool CharacterDeviceExists(VFSPath path);
            virtual bool BlockDeviceExists(VFSPath path);
            virtual bool SocketFileExists(VFSPath path);
            virtual bool FIFOFileExists(VFSPath path);
            virtual bool FileExists(VFSPath path);
            virtual bool SpecialFileExists(VFSPath path);
            virtual void CreateSymlink(VFSPath existingFile, VFSPath symlinkFile);
            virtual void CreateHardlink(VFSPath existingFile, VFSPath newName);
            virtual bool DirectoryExists(VFSPath path)=0;
            virtual void DeleteFile(VFSPath path)=0;
            virtual void DeleteDirectoryRecurse(VFSPath path);
            virtual VFSPathEnumerator EnumeratePaths(VFSPath path) = 0;
            virtual void MoveFile(VFSPath src, VFSPath dest)=0;
            virtual void MoveDirectory(VFSPath src, VFSPath dest);
            virtual VFSPath ReadLink(VFSPath path);
            virtual std::string VFSPathToSystem(VFSPath path)=0;
            virtual VFSPath SystemToVFSPath(std::string path)=0;


            virtual void GetDate(VFSPath path, Date::DateTime& lastWrite, Date::DateTime& lastAccess);
            virtual void SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess);

            virtual bool StatVFS(VFSPath path, StatVFSData& data);

            virtual void Chmod(VFSPath path, uint32_t mode);

            virtual void Lock(VFSPath path);
            virtual void Unlock(VFSPath path);
        
          
            virtual ~VFS();

            virtual void Close();

        protected:
              virtual std::shared_ptr<FSWatcher> CreateWatcher(std::shared_ptr<VFS> vfs, VFSPath path);
        friend class FSWatcher;
    };

    

    namespace Literals
    {
        inline VFSPath operator""_tpath(const char* path)
        {
            return VFSPath(path);
        }
    }
}

