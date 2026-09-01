#include "TessesFramework/TessesFramework.hpp"
#include <iostream>
using namespace Tesses::Framework;
using namespace Tesses::Framework::Http;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::Threading;

int main(int argc, char **argv) {
    TF_InitWithConsole();

    uint16_t port = 4269;

    Args args(argc, argv);
    for (auto &item : args.options) {
        if (item.first == "port")
            port = (uint16_t)std::stoul(item.second);
    }

    if (args.positional.empty()) {
        Console::WriteLine(args.filename + " [--port=] <url_root>");
        return 1;
    }

    Tesses::Framework::Http::HttpServer server(
        port, std::make_shared<ReverseProxyServer>(args.positional[0]));
    server.StartAccepting();
    TF_RunEventLoop();
    std::cout << "Closing server" << std::endl;
    TF_Quit();

    return 0;
}