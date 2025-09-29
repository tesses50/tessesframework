#include "TessesFramework/TessesFramework.hpp"

using namespace Tesses::Framework::Filesystem;

int main(int argc, char** argv)
{
    LocalFilesystem fs;
    VFSPath src = fs.SystemToVFSPath(argv[1]);
    VFSPath dest = fs.SystemToVFSPath(argv[2]);
    auto srcs = fs.OpenFile(src,"rb");
    auto dests = fs.OpenFile(dest,"wb");
    srcs->CopyTo(dests);

}