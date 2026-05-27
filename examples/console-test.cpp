#include <TessesFramework/TessesFramework.hpp>

using namespace Tesses::Framework;

int main(int argc, char** argv)
{
    TF_Init();
    Console::SetPosition(0,0);
    Console::WriteLine("Demi Lovato Is Cute Right");
    Console::WriteLine("Right");
    Console::WriteLine("Al Gore");
    Console::WriteLine("Demi Lovato");
    Console::WriteLine("Hello \x1B[33myellow\x1B[37m world");
    Console::ReadLine();

    Console::SetPosition(2,1);

    Console::ClearRetainPosition(ClearBehaviour::CB_CURSORANDBELOW);
    Console::ReadLine();
}