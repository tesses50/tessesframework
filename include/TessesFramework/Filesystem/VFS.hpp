#pragma once
#include "../Common.hpp"
#include "../Streams/Stream.hpp"
namespace Tesses::Framework::Filesystem
{
    class VFSPath {
        public:
            static VFSPath RelativeCurrentDirectory();
            bool relative;
            static std::vector<std::string> SplitPath(std::string path,bool native=false);
            std::vector<std::string> path;  
            VFSPath();
            VFSPath(std::vector<std::string> path);
            VFSPath(std::string path);
            VFSPath(VFSPath p, std::string subent);
            VFSPath(VFSPath p, VFSPath p2);
            VFSPath GetParent();
            VFSPath CollapseRelativeParents();
            std::string GetFileName();
            std::string ToString();
    };
    class VFS {
        public:
            
            virtual Tesses::Framework::Streams::Stream* OpenFile(VFSPath path, std::string mode)=0;
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
            virtual void GetPaths(VFSPath path, std::vector<VFSPath>& paths)=0;
            virtual void MoveFile(VFSPath src, VFSPath dest)=0;
            virtual void MoveDirectory(VFSPath src, VFSPath dest);
            virtual VFSPath ReadLink(VFSPath path);
            virtual std::string VFSPathToSystem(VFSPath path)=0;
            virtual VFSPath SystemToVFSPath(std::string path)=0;

            virtual ~VFS();
    };
}