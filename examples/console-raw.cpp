#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char** argv)
{
    using namespace Tesses::Framework;
    TF_Init();
    Console::WriteLine("Press q to quit");
    Console::SetEcho(false);
    Console::SetCanonical(false);
    while(int r = Console::Read())
    {
        if(r == 'q')
        {
            break;
        }
        Console::WriteLine(std::to_string(r));
    }
    Console::SetCanonical(true);
    Console::SetEcho(true);
}