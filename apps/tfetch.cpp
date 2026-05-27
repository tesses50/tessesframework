#include "TessesFramework/TessesFramework.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Http;
int main(int argc, char **argv) {
    if (argc < 3) {
        printf("USAGE: %s <url> <path>\n", argv[0]);
        printf("USAGE: %s <unixSocket> <url> <path>\n", argv[0]);
        return 1;
    }
    if (argc >= 4) {
        std::string unixSocket = argv[1];
        std::string url = argv[2];
        std::string path = argv[3];
        Tesses::Framework::Http::DownloadUnixSocketToFileSimple(unixSocket, url,
                                                                path);
    } else {
        std::string path = argv[2];
        Tesses::Framework::Http::DownloadToFileSimple(argv[1], path);
    }

    return 0;
}