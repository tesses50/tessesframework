#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char** argv)
{
    using namespace Tesses::Framework;
    TF_Init();
    Console::WriteLine("\"" + TF_GetExecutableName() + "\"");
    return 0;
}