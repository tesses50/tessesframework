#pragma once
#include "TorrentFile.hpp"
#include <queue>
#include "../Threading/Thread.hpp"
#include "../Random.hpp"
namespace Tesses::Framework::BitTorrent
{
    
    class ActiveTorrent;

    class TorrentBitField {
        std::vector<uint8_t> bits;
        size_t no_bits=0;
        public:
            TorrentBitField();
            TorrentBitField(size_t bits);
            bool get(size_t index);
            void set(size_t index, bool val);
            void zero();
            size_t size();
            void resize(size_t len);
            std::vector<uint8_t>& data();
            bool allone();
    };  
    struct CancelRequest {
        uint32_t piece;
        uint32_t begin;
        uint32_t length;
    };

    class TorrentPeer {
        public:
            bool isChokingMe;
            bool isChoked;
            bool intrested;
            std::shared_ptr<Tesses::Framework::Streams::NetworkStream> stream;
            std::vector<CancelRequest> cancel_requests;
            std::vector<uint8_t> messages;
            TorrentBitField has;

            std::vector<std::optional<TorrentBitField>> blocksRequested;
            std::string ip;
            uint16_t port;
    };

    class ActiveTorrent {
        public:
            Random rng;
            ActiveTorrent(TorrentFile file, std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,Tesses::Framework::Filesystem::VFSPath directory, std::string peer_id);
            std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs;
            std::map<std::string,uint64_t> udp_connection_ids;
            int64_t downloaded;
            int64_t uploaded;
            int64_t getLeft();
            void addPeer(std::string ip, uint16_t port);
            Tesses::Framework::Serialization::Bencode::BeString info_hash;
            std::string peer_id;
            time_t lastTime;
            bool mustAnnounce();
            void udpAnounce(Tesses::Framework::Http::Uri uri);
            void httpAnounce(std::string url);
            Tesses::Framework::Threading::Mutex mtx;
            std::vector<std::optional<TorrentBitField>> blocksAquired;
            TorrentBitField has;
            TorrentFile file;
            int64_t torrentSize; //to make it more efficient
            Tesses::Framework::Filesystem::VFSPath directory;
            std::vector<std::shared_ptr<TorrentPeer>> connections;
            std::shared_ptr<ReadWriteAt> torrent_disk;

            size_t pieceSize(size_t piece);
            std::array<uint8_t,1024> buffer;
            void process();
            bool processMessages(std::shared_ptr<TorrentPeer> peer);
    };
    class TorrentManager 
    {
        std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs;
        Tesses::Framework::Filesystem::VFSPath defaultDirectory;
        std::vector<std::shared_ptr<ActiveTorrent>> downloading;
        std::vector<std::shared_ptr<ActiveTorrent>> seeding;
        std::queue<std::pair<TorrentFile,Tesses::Framework::Filesystem::VFSPath>> torrentQueue;
        int torrentCount;
        std::atomic<bool> running=false;
        public:
            TorrentManager(std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, Tesses::Framework::Filesystem::VFSPath defaultDirectory, int torrentCount);
            void AddTorrent(TorrentFile file);
            void AddTorrent(TorrentFile file, Tesses::Framework::Filesystem::VFSPath directory);
            void Start();
            void Stop();
            ~TorrentManager();
    };
    std::string GeneratePeerId();

}