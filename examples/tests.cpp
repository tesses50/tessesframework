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

    std::cout << "Bitfield test" << std::endl;
    Tesses::Framework::BitTorrent::TorrentBitField bf(16);
    bf.set(7,false);
    bf.set(6,false);
    bf.set(5,true);
    bf.set(4,false);
    bf.set(3,true);
    bf.set(2,false);
    bf.set(1,true);
    bf.set(0,false);
    bf.set(15,false);
    bf.set(14,true);
    bf.set(13,false);
    bf.set(12,true);
    bf.set(11,false);
    bf.set(10,false);
    bf.set(9,false);
    bf.set(8,true);

    std::cout << std::string(bf.data().begin(),bf.data().end()) << std::endl;
}