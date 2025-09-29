#pragma once
#include "VFS.hpp"
#include "VFSFix.hpp"
#include "../Threading/Mutex.hpp"
#include <atomic>
namespace Tesses::Framework::Filesystem
{
    class MemoryEntry {
        public:
            std::string name;
            virtual ~MemoryEntry();
    };
    class MemoryFileData {
        public:
            MemoryFileData();
            Date::DateTime lastWrite;
            
            bool canAccess;
            size_t readers;
            std::vector<uint8_t> file;
    };

    class MemoryFile : public MemoryEntry
    {
        public:
            std::shared_ptr<MemoryFileData> data;
            ~MemoryFile();
    };

    class MemoryDirectory : public MemoryEntry
    {
        public:
            MemoryDirectory();
            Date::DateTime lastWrite;
            std::vector<MemoryEntry*> entries;
            ~MemoryDirectory();
    };

    class MemorySymlink : public MemoryEntry
    {
        public:
            Date::DateTime lastWrite;
            VFSPath linkedTo;
    };

    class MemoryFilesystemStream : public Streams::Stream
    {
        public:
            std::shared_ptr<Tesses::Framework::Threading::Mutex> mtx;
            std::shared_ptr<MemoryFileData> data;
            bool canRead;
            bool canWrite;
            bool canSeek;
            int64_t pos;
            MemoryFilesystemStream(std::shared_ptr<Tesses::Framework::Threading::Mutex> mtx, std::shared_ptr<MemoryFileData> data,bool canRead, bool canWrite, bool canSeek);
            size_t Read(uint8_t* buff, size_t sz);
            size_t Write(const uint8_t* buff, size_t sz);
            bool CanRead();
            bool CanWrite();
            bool CanSeek();
            int64_t GetPosition();
            void Flush();
            void Seek(int64_t pos, Streams::SeekOrigin whence);
            ~MemoryFilesystemStream();
    };


    class MemoryFilesystem : public VFS
    {
        std::shared_ptr<Tesses::Framework::Threading::Mutex> mtx;
        MemoryDirectory root;

        MemoryEntry* GetEntry(VFSPath path,bool followSymlink);
        public:
            MemoryFilesystem();
            std::shared_ptr<Tesses::Framework::Streams::Stream> OpenFile(VFSPath path, std::string mode);
            
            void CreateDirectory(VFSPath path);
            void DeleteDirectory(VFSPath path);
            bool RegularFileExists(VFSPath path);
            bool SymlinkExists(VFSPath path);
            bool DirectoryExists(VFSPath path);
           
            void DeleteFile(VFSPath path);
            void CreateSymlink(VFSPath existingFile, VFSPath symlinkFile);
            VFSPathEnumerator EnumeratePaths(VFSPath path);
            void CreateHardlink(VFSPath existingFile, VFSPath newName);
            
            void MoveFile(VFSPath src, VFSPath dest);
            
            void MoveDirectory(VFSPath src, VFSPath dest);
            VFSPath ReadLink(VFSPath path);
            std::string VFSPathToSystem(VFSPath path);
            VFSPath SystemToVFSPath(std::string path);
            void GetDate(VFSPath path, Date::DateTime& lastWrite, Date::DateTime& lastAccess);
            void SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess);
            ~MemoryFilesystem();
    };
};