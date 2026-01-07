#include "TessesFramework/TessesFramework.hpp"
using namespace Tesses::Framework;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Filesystem;
using namespace Tesses::Framework::Serialization::Bencode;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::BitTorrent;
int percent(ActiveTorrent& torrent)
{
    if(torrent.torrentSize == 0) return 100;
    auto left = torrent.getLeft();
    return 100-(int)(((double)left / (double)torrent.torrentSize)*100.0);
    

}
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

        VFSPath path;
        path.relative=true;
        ActiveTorrent active(file,Filesystem::LocalFS,path,GeneratePeerId());
        int lastPercent = 0;
        std::cout << "0%" << std::endl;
        while(TF_IsRunning())
        {
            active.process();

            int cur=percent(active);
            if(cur > lastPercent)
            {  
                lastPercent = cur;
                std::cout << "\r" << std::to_string(cur) << "%";
            }
        }
        
    }
    TF_Quit();
    return 0;
}