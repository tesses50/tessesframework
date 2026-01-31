#pragma once
#include "VFS.hpp"
#include "VFSFix.hpp"
            
namespace Tesses::Framework::Filesystem
{
    class LocalFilesystem : public VFS
    {
        public:
            
            std::shared_ptr<Tesses::Framework::Streams::Stream> OpenFile(VFSPath path, std::string mode);
            
            void CreateDirectory(VFSPath path);
            void DeleteDirectory(VFSPath path);
            bool RegularFileExists(VFSPath path);
            bool SymlinkExists(VFSPath path);
            bool CharacterDeviceExists(VFSPath path);
            bool BlockDeviceExists(VFSPath path);
            bool SocketFileExists(VFSPath path);
            bool FIFOFileExists(VFSPath path);
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

            bool StatVFS(VFSPath path, StatVFSData& vfsData);

            void Chmod(VFSPath path, uint32_t mode);

            void Lock(VFSPath path);
            void Unlock(VFSPath path);

            protected:
            std::shared_ptr<FSWatcher> CreateWatcher(std::shared_ptr<VFS> vfs, VFSPath path);

    };  
    extern std::shared_ptr<LocalFilesystem> LocalFS;
}