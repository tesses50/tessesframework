#include "TessesFramework/Filesystem/NullFilesystem.hpp"

namespace Tesses::Framework::Filesystem
{
    std::shared_ptr<Tesses::Framework::Streams::Stream> NullFilesystem::OpenFile(VFSPath path, std::string mode)
    {
        return nullptr;
    }


    VFSPathEnumerator NullFilesystem::EnumeratePaths(VFSPath path)
    {
        return VFSPathEnumerator();
    }
    std::string NullFilesystem::VFSPathToSystem(VFSPath path)
    {
        return path.ToString();
    }
    VFSPath NullFilesystem::SystemToVFSPath(std::string path)
    {
        return VFSPath(path);
    }

    bool NullFilesystem::Stat(VFSPath path, StatData& data)
    {
        return false;
    }
}
