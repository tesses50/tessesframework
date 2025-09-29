#include "TessesFramework/Filesystem/NullFilesystem.hpp"

namespace Tesses::Framework::Filesystem
{
    std::shared_ptr<Tesses::Framework::Streams::Stream> NullFilesystem::OpenFile(VFSPath path, std::string mode)
    {
        return nullptr;
    }
    void NullFilesystem::CreateDirectory(VFSPath path)
    {

    }
    void NullFilesystem::DeleteDirectory(VFSPath path)
    {

    }
    bool NullFilesystem::RegularFileExists(VFSPath path)
    {
        return false;
    }
    bool NullFilesystem::DirectoryExists(VFSPath path)
    {
        return false;
    }
    void NullFilesystem::DeleteFile(VFSPath path)
    {

    }
    VFSPathEnumerator NullFilesystem::EnumeratePaths(VFSPath path)
    {
        return VFSPathEnumerator();
    }
    void NullFilesystem::MoveFile(VFSPath src, VFSPath dest)
    {

    }
    std::string NullFilesystem::VFSPathToSystem(VFSPath path)
    {
        return path.ToString();
    }
    VFSPath NullFilesystem::SystemToVFSPath(std::string path)
    {
        return VFSPath(path);
    }
}