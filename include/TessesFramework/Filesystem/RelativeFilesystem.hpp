#pragma once
#include "VFS.hpp"
#include "VFSFix.hpp"

namespace Tesses::Framework::Filesystem
{
    class RelativeFilesystem : public VFS {
        Tesses::Framework::Threading::Mutex mtx;
        VFSPath working;
        std::shared_ptr<VFS> vfs;
        private:
            VFSPath ToParent(VFSPath path);
        public:
            RelativeFilesystem(std::shared_ptr<VFS> vfs, VFSPath working);
            VFSPath GetWorking();
            void SetWorking(VFSPath path);
            std::shared_ptr<VFS> GetVFS();

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

             void Chown(VFSPath path, uint32_t uid, uint32_t gid);
            void Chmod(VFSPath path, uint32_t mode);
            FIFOCreationResult CreateFIFO(VFSPath path, uint32_t mode);
            void Lock(VFSPath path);
            void Unlock(VFSPath path);
            protected:
                std::shared_ptr<FSWatcher> CreateWatcher(std::shared_ptr<VFS> vfs, VFSPath path);


                class Watcher : public FSWatcher {
                    std::shared_ptr<FSWatcher> watcher;
                    protected:
                        void SetEnabledImpl(bool enabled);
                    public:
                        Watcher(std::shared_ptr<RelativeFilesystem> vfs, VFSPath path);
                        ~Watcher();
                };
    };
}