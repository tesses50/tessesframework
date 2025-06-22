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
    printf("OPTIONS:\n-p PORT, --port PORT: Change port from 9852\n-l, --listing: Enable listing\n-s, --spa: Enable SPA mode (send \"/\" body instead of not found)\n-h, --help: This Screen\n");
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
    uint16_t port = 9852L;
    
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i],"--help") == 0 || strcmp(argv[i],"-h") == 0)
        {
            print_help(argv[0]);
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


    FileServer fs(directory,allowListing, spa);
    HttpServer server(port,fs);
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
