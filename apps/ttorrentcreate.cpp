#include "TessesFramework/TessesFramework.hpp"

using namespace Tesses::Framework;
using namespace Tesses::Framework::Streams;
using namespace Tesses::Framework::Filesystem;
using namespace Tesses::Framework::Serialization::Bencode;
using namespace Tesses::Framework::TextStreams;
using namespace Tesses::Framework::BitTorrent;
void usage(Args& args)
{
    std::cout << "USAGE: " << args.filename << " [options] torrent_contents torrent_file.torrent" << std::endl << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << "tracker:       torrent tracker url (require at least one)" << std::endl;
    std::cout << "webseed:       url for web seeding" << std::endl;
    std::cout << "comment:       set the comment" << std::endl;
    std::cout << "created_by:    set the created by field, defaults to TessesFrameworkTorrent" << std::endl;
    std::cout << "piece_length:  set the piece length, defaults to " + std::to_string(DEFAULT_PIECE_LENGTH) << std::endl << std::endl;
    std::cout << "FLAGS:" << std::endl;
    std::cout << "help:          bring this help message up" << std::endl;
    std::cout << "private:       enable private tracker flag" << std::endl;
    exit(1);
}
int main(int argc, char** argv)
{
    TF_Init();
    std::vector<BeString> webseeds;
    std::vector<BeString> trackers;
    bool isPrivate=false;
    int64_t pieceLength = DEFAULT_PIECE_LENGTH;
    std::string comment="";
    std::string created_by = "TessesFrameworkTorrent";

    Args args(argc, argv);
    if(args.positional.size() < 2)
        usage(args);

    for(auto& opt : args.options)
    {
        if(opt.first == "tracker")
            trackers.push_back(opt.second);
        else if(opt.first == "webseed")
            webseeds.push_back(opt.second);
        else if(opt.first == "comment")
            comment = opt.second;
        else if(opt.first == "created_by")
            created_by = opt.second;
        else if(opt.first == "piece_length")
            pieceLength = std::stoll(opt.second);
    }

    for(auto& flag : args.flags)
    {
        if(flag == "help")
            usage(args);
        if(flag == "private")
            isPrivate = true;
    }
    
    if(trackers.empty())
        usage(args);
    {
        auto torrent_file_stream = LocalFS->OpenFile(args.positional[1],"wb");
        TorrentFile::CreateTorrent(torrent_file_stream,trackers,webseeds,LocalFS, args.positional[0], isPrivate, pieceLength, comment, created_by);
    }
    return 0;
}