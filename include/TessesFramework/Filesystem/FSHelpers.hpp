#pragma once
#include "TempFS.hpp"
#include "VFSFix.hpp"

namespace Tesses::Framework::Filesystem::Helpers
{
    void ReadAllText(std::shared_ptr<VFS> vfs, VFSPath path, std::string& text);
    void ReadAllLines(std::shared_ptr<VFS> vfs, VFSPath path, std::vector<std::string>& lines);
    void ReadAllBytes(std::shared_ptr<VFS> vfs, VFSPath path, std::vector<uint8_t>& array);
    std::string ReadAllText(std::shared_ptr<VFS> vfs, VFSPath path);
    std::vector<std::string> ReadAllLines(std::shared_ptr<VFS> vfs, VFSPath path);
    std::vector<uint8_t> ReadAllBytes(std::shared_ptr<VFS> vfs, VFSPath path);
    void WriteAllText(std::shared_ptr<VFS> vfs, VFSPath path, const std::string& text);
    void WriteAllLines(std::shared_ptr<VFS> vfs, VFSPath path, const std::vector<std::string>& parts);
    void WriteAllBytes(std::shared_ptr<VFS> vfs, VFSPath path, const std::vector<uint8_t>& bytes);

    void CopyStreamProgress(std::shared_ptr<Streams::Stream> src,std::shared_ptr<Streams::Stream> dest, std::function<void(int64_t offset, int64_t length)> progress);

    void CopyFile(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc, std::shared_ptr<VFS> vfsDest, VFSPath pathDest, bool overwrite=true);
    void CopyFile(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc, std::shared_ptr<VFS> vfsDest, VFSPath pathDest, std::function<void(int64_t offset, int64_t length)> progress, bool overwrite=true);
    void CopyDirectory(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc, std::shared_ptr<VFS> vfsDest, VFSPath pathDest, bool overwrite=true);
    void CopyDirectory(std::shared_ptr<VFS> vfsSrc, VFSPath pathSrc, std::shared_ptr<VFS> vfsDest, VFSPath pathDest, std::function<void(int64_t offset, int64_t length, VFSPath currentFile)> progress, bool overwrite=true);

    inline std::shared_ptr<TempFS> CreateTempFS(bool deleteOnDestroy=true) 
    {
        return std::make_shared<TempFS>(deleteOnDestroy);
    }
    inline std::shared_ptr<TempFS> CreateTempFS(std::shared_ptr<VFS> vfs,bool deleteOnDestroy=true)
    {
        return std::make_shared<TempFS>(vfs,deleteOnDestroy);
    }

}