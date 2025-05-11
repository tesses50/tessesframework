#pragma once
#include "VFS.hpp"
#include "VFSFix.hpp"
            
namespace Tesses::Framework::Filesystem
{
    class LocalFilesystem : public VFS
    {
        public:
            
            Tesses::Framework::Streams::Stream* OpenFile(VFSPath path, std::string mode);
            
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

    };  
    extern LocalFilesystem LocalFS;
}