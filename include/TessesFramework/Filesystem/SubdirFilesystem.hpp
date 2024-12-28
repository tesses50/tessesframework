#pragma once
#include "VFS.hpp"
namespace Tesses::Framework::Filesystem
{
    class SubdirFilesystem : public VFS
    {
        bool owns;
        VFS* parent;
        VFSPath path;
        VFSPath ToParent(VFSPath path);
        VFSPath FromParent(VFSPath path);
        public:
            SubdirFilesystem(VFS* parent, VFSPath path, bool owns);
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
            void DeleteDirectoryRecurse(VFSPath path);
            VFSPath ReadLink(VFSPath path);
            std::string VFSPathToSystem(VFSPath path);
            VFSPath SystemToVFSPath(std::string path);
            ~SubdirFilesystem();
            void GetDate(VFSPath path, time_t& lastWrite, time_t& lastAccess);
            void SetDate(VFSPath path, time_t lastWrite, time_t lastAccess);
   
    };
}