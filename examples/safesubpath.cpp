#include "TessesFramework/TessesFramework.hpp"
#include <iostream>

using namespace Tesses::Framework::Filesystem;

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "USAGE: " << argv[0] << " <parent> <subpath>\n";
        return 1;
    }

    VFSPath parent(argv[1]);
    VFSPath subpath(argv[2]);
    VFSPath newPath(parent, subpath.CollapseRelativeParents());

    std::cout << newPath.ToString() << "\n";
}