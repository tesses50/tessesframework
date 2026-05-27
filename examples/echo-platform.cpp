#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char** argv)
{
    using namespace Tesses::Framework;
    TF_Init();
    Console::WriteLine(Tesses::Framework::Platform::Environment::GetPlatform());
    return 0;
}