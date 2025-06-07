#include "TessesFramework/TessesFramework.hpp"
using namespace Tesses::Framework;
int main(int argc, char** argv)
{
    TF_Init();
    auto event =  std::make_shared<FunctionalEvent<uint64_t>>([](uint64_t n)->void {
        std::cout << n << std::endl;
    });
    OnItteraton += event;


    auto event2 = std::make_shared<FunctionalEvent<uint64_t>>([&event](uint64_t n)->void{
        if(n == 10000) OnItteraton -= event;
    });
    OnItteraton += event2;


    TF_RunEventLoop();
    
    return 0;

}