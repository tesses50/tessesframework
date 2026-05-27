#include "TessesFramework/Common.hpp"
#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char **argv) {
    using namespace Tesses::Framework;
    TF_Init();

    auto timer = TF_Timer(
        []() -> void { std::cout << "Hi, I am a timer" << std::endl; });

    TF_RunEventLoop();

    TF_Quit();
}