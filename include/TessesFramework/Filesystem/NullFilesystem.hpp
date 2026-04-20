#pragma once
#include "VFS.hpp"
#include "VFSFix.hpp"

namespace Tesses::Framework::Filesystem
{
    class NullFilesystem : public VFS
    {
        public:
            std::shared_ptr<Tesses::Framework::Streams::Stream> OpenFile(VFSPath path, std::string mode);
            VFSPathEnumerator EnumeratePaths(VFSPath path);
            bool Stat(VFSPath path, StatData& data);
            std::string VFSPathToSystem(VFSPath path);
            VFSPath SystemToVFSPath(std::string path);
    };
}
