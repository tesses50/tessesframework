#pragma once
#include "../Common.hpp"
#include "VFS.hpp"
#include "VFSFix.hpp"
namespace Tesses::Framework::Filesystem
{
    void UniqueString(std::string& text);

    class TempFS : public VFS
    {
        std::shared_ptr<VFS> vfs;
        bool deleteOnDestroy;
        std::shared_ptr<VFS> parent;
        std::string tmp_str;
        public:
            std::string TempDirectoryName();
            TempFS(bool deleteOnDestroy=true);
            TempFS(std::shared_ptr<VFS> vfs,bool deleteOnDestroy=true);

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

            void SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess);
             bool StatVFS(VFSPath path, StatVFSData& vfsData);
             bool Stat(VFSPath path, StatData& data);

            void Chmod(VFSPath path, uint32_t mode);

            void Chown(VFSPath path, uint32_t uid, uint32_t gid);
            FIFOCreationResult CreateFIFO(VFSPath path, uint32_t mode);
            void Close();

            void Lock(VFSPath path);
            void Unlock(VFSPath path);
            ~TempFS();
    };
}
