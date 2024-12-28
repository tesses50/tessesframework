#include "TessesFramework/TessesFramework.hpp"
#include <iostream>

using namespace Tesses::Framework;
using namespace Tesses::Framework::Filesystem;
using namespace Tesses::Framework::Streams;

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("USAGE: %s <command> <args...>\n",argv[0]);
        return 1;
    }

    LocalFilesystem lfs;

    std::string root = "./root";
    std::string mountDemi = "./demi";
    std::string mountJoelSlashJim = "./joelslashjim";

    SubdirFilesystem rootdir(&lfs,root,false);

    SubdirFilesystem mountDemidir(&lfs,mountDemi,false);


    SubdirFilesystem mountjohnslashjim(&lfs,mountJoelSlashJim,false);

    MountableFilesystem fs(&rootdir,false);
    fs.Mount(std::string("/demi"), &mountDemidir,false);

    fs.Mount(std::string("/joel/jim"), &mountjohnslashjim,false);

    std::string command = argv[1];


    if(command == "ls")
    {
        std::string dir = "/";
        if(argc > 2) dir = argv[2];
        
        for(auto item : fs.EnumeratePaths(dir))
        {
            std::cout << item.GetFileName() << std::endl;
        }
    }
    else if(command == "cat")
    {
        FileStream strm(stdout, false,"wb",false);
        for(int a = 2; a < argc; a++)
        {
            std::string path = argv[a];
            auto f = fs.OpenFile(path,"rb");
            if(f != nullptr)
            {
                f->CopyTo(strm);
                delete f;
            }
        }
    }
    
}