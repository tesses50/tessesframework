#include "TessesFramework/Filesystem/VFS.hpp"
#include <iostream>
#include <string>
using namespace Tesses::Framework::Filesystem;

int main(int argc, char **argv) {
    VFSPath path("C:/home/user/file");
    VFSPath path2("D:/home/user/file");
    VFSPath path3("C:/home/user");
    std::cout << path.MakeRelative(path2).ToString() << std::endl;

    std::cout << (path3 / path.MakeRelative(path2) / "John")
                     .CollapseRelativeParents()
                     .ToString()
              << std::endl;
    return 0;
}