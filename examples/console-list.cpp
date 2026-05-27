#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char** argv)
{
    using namespace Tesses::Framework;
    using namespace Tesses::Framework::TextStreams;
    TF_Init();

    std::vector<std::string> myList = {
        "Demi Lovato",
        "The Gremlins",
        "Al Gore",
        "Steve Ballmer",
        "CrossLang",
        "Tom Scott",
        "Louis Rossmann",
        "Mike Nolan",
        "Tim Cook"
    };
    
    auto res = Console::List(myList);

    if(res > myList.size())
    {
        Console::WriteLine("You must select one");
    }
    else {
        StdOut() << "You selected " << myList[res] << "." << NewLine();
    }
    return 0;
}