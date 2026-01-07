#include "TessesFramework/TessesFramework.hpp"
using namespace Tesses::Framework;
using namespace Tesses::Framework::BitTorrent;
using namespace Tesses::Framework::Serialization::Bencode;
using namespace Tesses::Framework::Filesystem;
int main(int argc, char** argv)
{
    TF_Init();
    if(argc < 2)
    {
        printf("USAGE: %s torrent_file\n",argv[0]);
        return 1;
    }
    auto strm = LocalFS->OpenFile((std::string)argv[1],"rb");
    auto data = Bencode::Load(strm);
    if(std::holds_alternative<BeDictionary>(data)){
        std::get<BeDictionary>(data).SetValue("url-list",BeUndefined());
    }
    strm = LocalFS->OpenFile((std::string)argv[1],"wb");
    Bencode::Save(strm,data);
    return 0;
}