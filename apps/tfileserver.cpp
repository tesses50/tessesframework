#include "TessesFramework/TessesFramework.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Http;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::Threading;

void print_help(const char* name)
{
    printf("Tesses FileServer\nUSAGE: %s [OPTIONS] <dir>\n",name);
    printf("OPTIONS:\n-p PORT, --port PORT: Change port from 9852\n-l, --listing: Enable listing\n-s, --spa: Enable SPA mode (send \"/\" body instead of not found)\n-c, --cgi-bin: Enable cgi (common gateway interface) support (specify a folder like /cgi-bin)\n-a, --cgi-admin: cgi admin email\n-w, --cgi-working: working directory for cgi scripts\n-h, --help: This Screen\n");
    exit(1);
}
int main(int argc, char** argv)
{
    try {

    TF_InitWithConsole();
    TF_LOG("INIT");
    const char* directory = "wwwroot";
    bool spa=false;
    bool allowListing = false;
    std::optional<std::string> cgi_bin;
    std::optional<std::string> cgi_admin;
    std::optional<Tesses::Framework::Filesystem::VFSPath> cgi_workdir;
    uint16_t port = 9852L;
    
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i],"--help") == 0 || strcmp(argv[i],"-h") == 0)
        {
            print_help(argv[0]);
        }
        else if(strcmp(argv[i], "-c") == 0 || strcmp(argv[i],"--cgi-bin") == 0)
        {
            if(i+1>=argc)
            {
                printf("ERROR: Not enough arguments for cgi-bin\n");
                print_help(argv[0]);
            }
            else {
                printf("CGI is enabled\n");
                cgi_bin = argv[++i];
            }
        }
        else if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i],"--cgi-admin") == 0)
        {
            if(i+1>=argc)
            {
                printf("ERROR: Not enough arguments for cgi-admin\n");
                print_help(argv[0]);
            }
            else {
                cgi_admin = argv[++i];
            }
        }
        else if(strcmp(argv[i], "-w") == 0 || strcmp(argv[i],"--cgi-working") == 0)
        {
            if(i+1>=argc)
            {
                printf("ERROR: Not enough arguments for cgi-working\n");
                print_help(argv[0]);
            }
            else {
                cgi_workdir = (Tesses::Framework::Filesystem::VFSPath)argv[++i];
            }
        }
        
        else if(strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--listing") == 0)
        {
            allowListing = true;
        }
        else if(strcmp(argv[i],"-s") == 0 || strcmp(argv[i],"--spa") == 0)
        {
            spa=true;
        }
        else if(strcmp(argv[i],"-p") == 0 || strcmp(argv[i],"--port") == 0)
        {
            if(i+1>=argc) 
            {
                printf("ERROR: Not enough arguments for PORT\n");
                print_help(argv[0]);
            }
            else
            {

                port = (uint16_t)std::stoul(argv[++i]);
            }
        }
        else
        {
            directory = argv[i];
        }
    }
    
    std::cout << "In folder: " << std::filesystem::absolute(directory).string() << std::endl;
    std::shared_ptr<IHttpServer> http;
    
    auto fs = std::make_shared<FileServer>(directory,allowListing, spa);
    if(cgi_bin)
    {
        Tesses::Framework::Filesystem::VFSPath dir = *cgi_bin;
        dir.relative = true;
        auto svr = std::make_shared<MountableServer>(fs);
        auto cgi = std::make_shared<CGIServer>(std::filesystem::absolute(directory).string() / dir);
        cgi->adminEmail = cgi_admin;
        cgi->workingDirectory = cgi_workdir;
        svr->Mount(*cgi_bin, cgi);
        http = svr;
    }
    else {
        http = fs;
    }

    HttpServer server(port,http);
    server.StartAccepting();
    TF_RunEventLoop();
    std::cout << "Closing server" << std::endl;
    TF_Quit();
}catch(std::exception& ex)
{
    TF_LOG(ex.what());
}
    return 0;
}
