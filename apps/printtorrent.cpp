#include "TessesFramework/TessesFramework.hpp"

using namespace Tesses::Framework;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Filesystem;
using namespace Tesses::Framework::Serialization::Bencode;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::BitTorrent;
int main(int argc, char** argv)
{
    TF_Init();
    if(argc < 2)
    {
        printf("USAGE: %s /path/to/torrent/file\n",argv[0]);
        return 1;
    }
    auto strm = LocalFS->OpenFile((std::string)argv[1],"rb");
    auto bencode = Bencode::Load(strm);
    if(std::holds_alternative<BeDictionary>(bencode))
    {   

        TorrentFile file(std::get<BeDictionary>(bencode));
        file.Print(std::make_shared<ConsoleWriter>());
    }
    return 0;
}