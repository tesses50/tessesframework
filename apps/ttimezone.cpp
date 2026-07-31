#include <TessesFramework/TessesFramework.hpp>

int main(int argc, char **argv) {
    Tesses::Framework::TF_InitWithConsole();
    std::cout << Tesses::Framework::Date::GetTimeZone() << std::endl;
    return 0;
}