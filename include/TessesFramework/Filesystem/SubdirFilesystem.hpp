#pragma once
#include "VFS.hpp"
#include "VFSFix.hpp"

namespace Tesses::Framework::Filesystem
{
    class SubdirFilesystem : public VFS
    {
        std::shared_ptr<VFS> parent;
        VFSPath path;
        VFSPath ToParent(VFSPath path);
        VFSPath FromParent(VFSPath path);
        public:
            SubdirFilesystem(std::shared_ptr<VFS> parent, VFSPath path);
            std::shared_ptr<Tesses::Framework::Streams::Stream> OpenFile(VFSPath path, std::string mode);
            void CreateDirectory(VFSPath path);
            void DeleteDirectory(VFSPath path);
            void DeleteFile(VFSPath path);
            void CreateSymlink(VFSPath existingFile, VFSPath symlinkFile);
            VFSPathEnumerator EnumeratePaths(VFSPath path);
            void CreateHardlink(VFSPath existingFile, VFSPath newName);
            void MoveFile(VFSPath src, VFSPath dest);
            void MoveDirectory(VFSPath src, VFSPath dest);
            void DeleteDirectoryRecurse(VFSPath path);
            VFSPath ReadLink(VFSPath path);
            std::string VFSPathToSystem(VFSPath path);
            VFSPath SystemToVFSPath(std::string path);
            ~SubdirFilesystem();
            void SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess);
             bool StatVFS(VFSPath path, StatVFSData& vfsData);
             bool Stat(VFSPath path, StatData& data);

             void Chown(VFSPath path, uint32_t uid, uint32_t gid);
            void Chmod(VFSPath path, uint32_t mode);
            FIFOCreationResult CreateFIFO(VFSPath path, uint32_t mode);
            void Lock(VFSPath path);
            void Unlock(VFSPath path);
    };
}
