#include "TessesFramework/TessesFramework.hpp"
int main(int argc, char** argv)
{
    Tesses::Framework::TF_Init();
    std::cout << "Basic auth test" << std::endl;
    std::string theEncoded="dXNlcjpwYXNz";
    Tesses::Framework::Http::ServerContext ctx(nullptr);
    ctx.requestHeaders.SetValue("Authorization","Basic " + theEncoded);
    std::string user;
    std::string pass;
    std::cout << Tesses::Framework::Http::BasicAuthServer::GetCreds(ctx,user,pass) << std::endl;
    std::cout << user << std::endl;
    std::cout << pass << std::endl;
 
}