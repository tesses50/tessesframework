#include "TessesFramework/Serialization/Json.hpp"

using namespace Tesses::Framework::Serialization::Json;

int main(int argc, char** argv)
{
    auto json = Json::Decode("\"\\uD83D\\uDE44\"");
    std::string str;
    if(TryGetJToken(json,str))
    {
        std::cout << str << std::endl;
    }
}