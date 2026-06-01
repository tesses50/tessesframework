#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char **argv) {
    Tesses::Framework::TF_Init();

    Tesses::Framework::Platform::ShellFileOrUrl("https://tesses.net/");
    return 0;
}