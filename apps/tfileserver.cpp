#include "TessesFramework/TessesFramework.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Http;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::Threading;

int main(int argc, char** argv)
{
    TF_Init();
    if(argc < 2)
    {
        printf("USAGE: %s <dir>\n",argv[0]);
        return 0;
    }
    FileServer fs(argv[1],true,false);
    HttpServer server(10000U,fs);
    server.StartAccepting();
    TF_RunEventLoop();
    std::cout << "Closing server" << std::endl;
    return 0;
}