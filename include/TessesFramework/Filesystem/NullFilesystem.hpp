#pragma once
#include "VFS.hpp"
#include "VFSFix.hpp"
            
namespace Tesses::Framework::Filesystem
{
    class NullFilesystem : public VFS
    {
        public:  
            Tesses::Framework::Streams::Stream* OpenFile(VFSPath path, std::string mode);
            void CreateDirectory(VFSPath path);
            void DeleteDirectory(VFSPath path);
            bool RegularFileExists(VFSPath path);
            bool DirectoryExists(VFSPath path);
            void DeleteFile(VFSPath path);
            VFSPathEnumerator EnumeratePaths(VFSPath path);
            void MoveFile(VFSPath src, VFSPath dest);
            std::string VFSPathToSystem(VFSPath path);
            VFSPath SystemToVFSPath(std::string path);
    };
}