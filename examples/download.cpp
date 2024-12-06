#include "TessesFramework/TessesFramework.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Http;
int main(int argc, char** argv)
{
    if(argc < 3)
    {
        printf("USAGE: %s <url> <path>\n",argv[0]);
        return 1;
    }
    FileStream strm(argv[2],"wb");

    HttpRequest req;
    req.url = argv[1];
    HttpResponse resp(req);
    resp.CopyToStream(&strm);

    
    return 0;
}