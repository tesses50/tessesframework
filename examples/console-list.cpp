#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char **argv) {
    using namespace Tesses::Framework;
    using namespace Tesses::Framework::TextStreams;
    TF_InitWithConsole();

    std::vector<std::string> myList = {
        "Demi Lovato",    "The Gremlins", "Al Gore",
        "Steve Ballmer",  "CrossLang",    "Tom Scott",
        "Louis Rossmann", "Mike Nolan",   "Tim Cook"};

    auto res = Console::List(myList);

    if (res > myList.size()) {
        Console::WriteLine("You must select one");
        TF_RunEventLoop();
    } else {
        StdOut() << "You selected " << myList[res] << "." << NewLine();
        TF_RunEventLoop();
    }
    return 0;
}