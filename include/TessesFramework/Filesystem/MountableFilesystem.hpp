#pragma once
#include "VFS.hpp"
#include "VFSFix.hpp"
            
namespace Tesses::Framework::Filesystem
{
    class MountableDirectory {
        public:
            std::string name;
            VFS* vfs;
            bool owns;
            std::vector<MountableDirectory*> dirs;
            void GetFS(VFSPath srcPath, VFSPath curDir, VFSPath& destRoot, VFSPath& destPath, VFS*& vfs);
            ~MountableDirectory();
    };

    class MountableFilesystem : public VFS
    {
        bool owns;
        VFS* root;

        std::vector<MountableDirectory*> directories;
        
        void GetFS(VFSPath srcPath, VFSPath& destRoot, VFSPath& destPath, VFS*& vfs);

    
        public:
            MountableFilesystem();
            MountableFilesystem(VFS* root, bool owns);
            void Mount(VFSPath path, VFS* fs, bool owns);
            void Unmount(VFSPath path);
            Tesses::Framework::Streams::Stream* OpenFile(VFSPath path, std::string mode);
            void CreateDirectory(VFSPath path);
            void DeleteDirectory(VFSPath path);
            bool SpecialFileExists(VFSPath path);
            bool FileExists(VFSPath path);
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
            ~MountableFilesystem();
            void GetDate(VFSPath path, time_t& lastWrite, time_t& lastAccess);
            void SetDate(VFSPath path, time_t lastWrite, time_t lastAccess);
    };
}