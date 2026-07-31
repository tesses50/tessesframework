#include "TessesFramework/TessesFramework.hpp"

int main(int argc, char **argv) {
    using namespace Tesses::Framework;
    TF_InitWithConsole();

    Console::Write("Username: ");

    std::string username = Console::ReadLine();

    Console::Write("Password: ");

    std::string password = Console::ReadPassword();

    Console::WriteLine("Your username was: \"" + username + "\", " +
                       std::to_string(username.size()));
    Console::WriteLine("Your password was: \"" + password + "\", " +
                       std::to_string(password.size()));

    TF_RunEventLoop();
}