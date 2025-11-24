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
            
            void GetDate(VFSPath path, Date::DateTime& lastWrite, Date::DateTime& lastAccess);
            void SetDate(VFSPath path, Date::DateTime lastWrite, Date::DateTime lastAccess);
             bool StatVFS(VFSPath path, StatVFSData& vfsData);

            void Chmod(VFSPath path, uint32_t mode);
            void Close();
            ~TempFS();
    };
}