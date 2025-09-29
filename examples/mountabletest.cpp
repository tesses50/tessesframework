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


    std::string root = "./root";
    std::string mountDemi = "./demi";
    std::string mountJoelSlashJim = "./joelslashjim";

    std::shared_ptr<SubdirFilesystem> rootdir = std::make_shared<SubdirFilesystem>(LocalFS,root);

    std::shared_ptr<SubdirFilesystem> mountDemidir = std::make_shared<SubdirFilesystem>(LocalFS,mountDemi);


    std::shared_ptr<SubdirFilesystem> mountjohnslashjim = std::make_shared<SubdirFilesystem>(LocalFS,mountJoelSlashJim);

    std::shared_ptr<MountableFilesystem> fs = std::make_shared<MountableFilesystem>(rootdir);
    fs->Mount(std::string("/demi"), mountDemidir);

    fs->Mount(std::string("/joel/jim"), mountjohnslashjim);

    std::string command = argv[1];


    if(command == "ls")
    {
        std::string dir = "/";
        if(argc > 2) dir = argv[2];
        
        for(auto item : fs->EnumeratePaths(dir))
        {
            std::cout << item.GetFileName() << std::endl;
        }
    }
    else if(command == "cat")
    {
        std::shared_ptr<FileStream> strm = std::make_shared<FileStream>(stdout, false,"wb",false);
        for(int a = 2; a < argc; a++)
        {
            std::string path = argv[a];
            auto f = fs->OpenFile(path,"rb");
            if(f != nullptr)
            {
                f->CopyTo(strm);
                
            }
        }
    }
    
}