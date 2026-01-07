#include "TessesFramework/BitTorrent/TorrentManager.hpp"
#include "TessesFramework/Serialization/BitConverter.hpp"
#include "TessesFramework/Crypto/Crypto.hpp"
#include "TessesFramework/Random.hpp"

namespace Tesses::Framework::BitTorrent
{
    
    TorrentBitField::TorrentBitField()
    {

    }
    TorrentBitField::TorrentBitField(size_t len)
    {
        resize(len);
    }
    size_t TorrentBitField::size()
    {
        return this->no_bits;
    }
    void TorrentBitField::resize(size_t len)
    {
        this->no_bits = len;
        size_t no = len / 8;
        if((len % 8) != 0)
            no++;
        this->bits.resize(no);
    }
    bool TorrentBitField::get(size_t bit)
    {
        size_t byte = bit / 8;
        size_t byteBit = 7-bit % 8;

        return (this->bits.at(byte) >> byteBit) & 1;
    }
    bool TorrentBitField::allone()
    {
        
        for(size_t i = 0; i < this->no_bits; i++)
        {
            size_t byte = i / 8;
            size_t byteBit = i % 8;
            if(((this->bits.at(byte) >> byteBit) & 1) == 0) return false;
        }
        return true;
    }
    void TorrentBitField::set(size_t bit, bool val)
    {
        size_t byte = bit / 8;
        size_t byteBit = 7-bit % 8;


        if(val)
        {
            this->bits.at(byte) |= 1 << byteBit;
        }
        else
        {
            this->bits.at(byte) &= ~(1 << byteBit);
        }
    }
    void TorrentBitField::zero()
    {
        std::fill(this->bits.begin(),this->bits.end(),0);
    }
    std::vector<uint8_t>& TorrentBitField::data()
    {
        return this->bits;
    }


    void TorrentManager::Start()
    {
        
    }
    bool ActiveTorrent::mustAnnounce()
    {
        time_t curTime = time(NULL);
        if(curTime < lastTime || (lastTime + 1800) <= curTime)
        {
            std::cout << "Must anounce" << std::endl;
            this->lastTime = curTime;
            return true;
        }
        return false;
    }
    std::string bt_handshake_str = "BitTorrent protocol";
    void ActiveTorrent::addPeer(std::string ip, uint16_t port)
    {
        std::cout << "Got peer: " << ip << ":" << port << std::endl;
        if(port == 0) return;
        for(auto conn : this->connections)
        {
            if(conn->ip == ip && conn->port == port) return;
        }
        auto peer = std::make_shared<TorrentPeer>();
        peer->ip = ip;
        peer->port = port;
        peer->isChoked=true;
        peer->isChokingMe=true;
        peer->intrested=false;
        peer->has.resize(this->has.size());
        peer->blocksRequested.resize(this->has.size());
        peer->stream = std::make_shared<Tesses::Framework::Streams::NetworkStream>(ip,port,false,false,false);
        
        std::vector<uint8_t> handshake;
        handshake.resize(49+bt_handshake_str.size());
        handshake[0] = (uint8_t)bt_handshake_str.size();
        std::copy(bt_handshake_str.begin(),bt_handshake_str.end(),handshake.begin()+1);
        auto off = bt_handshake_str.size()+1;
        for(size_t i = 0; i < 8; i++)
            handshake[off+i] = 0;
        std::copy(this->info_hash.data.begin(),this->info_hash.data.end(),handshake.begin()+off+8);
        std::copy(this->peer_id.begin(),this->peer_id.end(),handshake.begin()+off+28);
        peer->stream->WriteBlock(handshake.data(),handshake.size());
        handshake.resize(5+this->has.data().size());
        Serialization::BitConverter::FromUint32BE(handshake[0],this->has.data().size());
        handshake[4] = 5;
        
        std::copy(this->has.data().begin(),this->has.data().end(),handshake.begin()+5);
        peer->stream->WriteBlock(handshake.data(),handshake.size());
       
        Serialization::BitConverter::FromUint32BE(handshake[0],1);
        handshake[4] = 2;

        peer->stream->WriteBlock(handshake.data(),5);
        std::cout << "Handshake send complete for: " << ip << ":" << port << std::endl;


        this->connections.push_back(peer);
    }
    void ActiveTorrent::udpAnounce(Tesses::Framework::Http::Uri uri)
    {
        std::string connUrl = uri.HostPort();
        std::cout << "udp anouncing via: " << connUrl << std::endl;
        Streams::NetworkStream strm(uri.host,uri.port,true,false,false);
        
        auto trans_id = rng.Next(0xFFFFFFFF);
        if(this->udp_connection_ids.count(connUrl) == 0)
        {
           

           Serialization::BitConverter::FromUint64BE(this->buffer[0],0x41727101980);
           Serialization::BitConverter::FromUint32BE(this->buffer[8],0);
           Serialization::BitConverter::FromUint32BE(this->buffer[12],trans_id);

          std::cout << "writing ze data" << std::endl;
           strm.Write(this->buffer.data(),16);
            std::cout << "reading ze data" << std::endl;
            if(!strm.DataAvailable(5000)) {
                std::cout << "timeout" << std::endl;
                return;
            }
           if(strm.Read(this->buffer.data(),this->buffer.size()) != 16) return;
            std::cout << "read ze data" << std::endl;
           if(Serialization::BitConverter::ToUint32BE(this->buffer[0]) == 0 && Serialization::BitConverter::ToUint32BE(this->buffer[4]) == trans_id)
           {
                this->udp_connection_ids[connUrl] = Serialization::BitConverter::ToUint64BE(this->buffer[8]);

                trans_id = rng.Next(0xFFFFFFFF);
           } else return;
        }
        Serialization::BitConverter::FromUint64BE(this->buffer[0],this->udp_connection_ids[connUrl]);
        Serialization::BitConverter::FromUint32BE(this->buffer[8],1);
        Serialization::BitConverter::FromUint32BE(this->buffer[12],trans_id);
        std::copy(this->info_hash.data.begin(),this->info_hash.data.end(),this->buffer.begin()+16);

        std::copy(this->peer_id.begin(),this->peer_id.end(),this->buffer.begin()+36);
        Serialization::BitConverter::FromUint64BE(this->buffer[56], this->downloaded);
        Serialization::BitConverter::FromUint64BE(this->buffer[64], this->getLeft());
        Serialization::BitConverter::FromUint64BE(this->buffer[72], this->uploaded);
        Serialization::BitConverter::FromUint32BE(this->buffer[80],0);
        Serialization::BitConverter::FromUint32BE(this->buffer[84],0);
        Serialization::BitConverter::FromUint32BE(this->buffer[88],0);
        Serialization::BitConverter::FromUint32BE(this->buffer[92],1); //peers
        Serialization::BitConverter::FromUint16BE(this->buffer[96],0);
        strm.Write(this->buffer.data(),98);
        if(!strm.DataAvailable(5000)) {
                std::cout << "timeout" << std::endl;
                return;
        }
        size_t read = strm.Read(this->buffer.data(),this->buffer.size());
        if(read < 20) return;
        if(Serialization::BitConverter::ToUint32BE(this->buffer[0]) == 1 && Serialization::BitConverter::ToUint32BE(this->buffer[4]) == trans_id)
        {
            std::cout << "Found " << ((read - 20) / 6) << " peers" << std::endl;
            for(size_t peerIdx = 20; peerIdx + 6 <= read; peerIdx+=6)
            {
                std::string ip = std::to_string((uint32_t)this->buffer[peerIdx]);
                ip += "." + std::to_string((uint32_t)this->buffer[peerIdx+1]);
                ip += "." + std::to_string((uint32_t)this->buffer[peerIdx+2]);
                ip += "." + std::to_string((uint32_t)this->buffer[peerIdx+3]);

                uint16_t port = Tesses::Framework::Serialization::BitConverter::ToUint16BE(this->buffer[peerIdx+4]);

                addPeer(ip,port);
            }
        }

    }
    int64_t ActiveTorrent::getLeft()
    {
        int64_t left = 0;
        size_t normalPieceSize = pieceSize(0);
        size_t lastPieceSize = pieceSize(this->has.size()-1);
        for(size_t i = 0;i < this->has.size(); i++)
        {
            if(!this->has.get(i))
                if(i == this->has.size() - 1)
                    left += lastPieceSize;
                else
                    left += normalPieceSize;
        }
        return left;
    }
    void ActiveTorrent::httpAnounce(std::string url)
    {
        std::string newUrl = url + "?info_hash=" + Http::HttpUtils::UrlEncode((std::string)this->info_hash) + "&peer_id=" + Http::HttpUtils::UrlEncode(this->peer_id) + "&uploaded=" + std::to_string(this->uploaded) + "&downloaded=" + std::to_string(this->downloaded) + "&left=" + std::to_string(getLeft()) + "&numwant=10&compact=1&no_peer_id=1&port=0";
        Http::HttpRequest req;
        req.url= newUrl;
        req.method = "GET";
        Http::HttpResponse resp(req);
        if(resp.statusCode == 200)
        {
            auto bencode=Tesses::Framework::Serialization::Bencode::Bencode::Load(resp.ReadAsStream());
            if(std::holds_alternative<Tesses::Framework::Serialization::Bencode::BeDictionary>(bencode))
            {
                auto& dict = std::get<Tesses::Framework::Serialization::Bencode::BeDictionary>(bencode);
                auto peers = dict.GetValue("peers");
                if(std::holds_alternative<Tesses::Framework::Serialization::Bencode::BeString>(peers))
                {
                    //this is compact
                    auto compactPeers=std::get<Tesses::Framework::Serialization::Bencode::BeString>(peers);
                    for(size_t peerIdx = 0; peerIdx + 6 <= compactPeers.data.size(); peerIdx+=6)
                    {
                        std::string ip = std::to_string((uint32_t)compactPeers.data[peerIdx]);
                        ip += "." + std::to_string((uint32_t)compactPeers.data[peerIdx+1]);
                        ip += "." + std::to_string((uint32_t)compactPeers.data[peerIdx+2]);
                        ip += "." + std::to_string((uint32_t)compactPeers.data[peerIdx+3]);

                        uint16_t port = Tesses::Framework::Serialization::BitConverter::ToUint16BE(compactPeers.data[peerIdx+4]);

                        addPeer(ip,port);
                    }
                }
                else 
                {
                    auto normalPeers = std::get<Tesses::Framework::Serialization::Bencode::BeArray>(peers);
                    for(auto& item : normalPeers.tokens)
                    {
                        if(std::holds_alternative<Tesses::Framework::Serialization::Bencode::BeDictionary>(item))
                        {
                            auto& dict2=std::get<Tesses::Framework::Serialization::Bencode::BeDictionary>(item);
                            auto ip = dict2.GetValue("ip");
                            auto port = dict2.GetValue("port");
                            if(std::holds_alternative<Tesses::Framework::Serialization::Bencode::BeString>(ip) && std::holds_alternative<int64_t>(port))
                            {
                                addPeer((std::string)std::get<Tesses::Framework::Serialization::Bencode::BeString>(ip),(uint16_t)(uint64_t)std::get<int64_t>(port));
                            }

                        }
                    }
                }
            }
        }
    }

    void ActiveTorrent::process()
    {
        if(mustAnnounce())
        {
            std::vector<Tesses::Framework::Serialization::Bencode::BeString> announces;
            if(this->file.announce_list.empty())
                announces.push_back(this->file.announce);
            announces.insert(announces.end(),this->file.announce_list.begin(),this->file.announce_list.end());

            for(auto& a : announces)
            {
                Tesses::Framework::Http::Uri uri;
                if(Tesses::Framework::Http::Uri::TryParse(a,uri))
                {
                    if(uri.scheme == "udp:")
                    {
                        udpAnounce(uri);
                    }
                    else 
                    {
                        httpAnounce(a);
                    }
                }
            }
            std::cout << "end of anounces" << std::endl;
        }


        
        for(auto item : this->connections)
        {
             if(!item->isChokingMe)
            {
            for(size_t piece = 0; piece < this->has.size(); piece++)
            {
                if(!this->has.get(piece) && item->has.get(piece))
                {
                    bool hasFound=false;
                    //find this piece in bitfields
                    //find a block we don't have and that we never requested
                    //we need to ensure that both the peer and I have the bitfields for the piece

                    size_t _pieceSize = pieceSize(piece);
                    auto blockCount = _pieceSize / 16384;
                    if((_pieceSize % 16384) != 0) blockCount++;
                    if(!this->blocksAquired[piece] )
                    {
                        this->blocksAquired[piece] =TorrentBitField(blockCount);
                    }
                    if(!item->blocksRequested[piece] )
                    {
                        item->blocksRequested[piece] =TorrentBitField(blockCount);
                    }

                    for(size_t block = 0; block < blockCount; block++)
                    {
                        if(!this->blocksAquired[piece]->get(block) && !item->blocksRequested[piece]->get(block))
                        {
                            item->blocksRequested[piece]->set(block,true);
                            hasFound=true;

                            std::vector<uint8_t> msg((size_t)17);
                            Serialization::BitConverter::FromUint32BE(msg[0],13);
                            msg[4] = 6;
                            Serialization::BitConverter::FromUint32BE(msg[5], (uint32_t)piece);
                            
                            Serialization::BitConverter::FromUint32BE(msg[9], (uint32_t)(16384*block));
                            size_t readBlockSize = 16384;
                            if((_pieceSize % 16384) != 0 && block >= blockCount - 1) readBlockSize = _pieceSize % 16384;
                            
                            Serialization::BitConverter::FromUint32BE(msg[13], (uint32_t)(readBlockSize));

                            item->stream->WriteBlock(msg.data(),msg.size());

                            std::cout << "Request block: " << block << " of piece: " << piece << " from peer: " << item->ip << ":" << item->port << std::endl;

                            break;
                        }
                    }
                    if(hasFound) break;
                }
            }
        }
             else std::cout << "Is choking" << std::endl;
            if(item->stream->DataAvailable(1))
            {
                std::cout << "Has message" << std::endl;
                size_t read = item->stream->Read(buffer.data(),buffer.size());
                item->messages.insert(item->messages.cend(),buffer.begin(),buffer.begin()+read);
                if(!processMessages(item))
                {
                    std::cout << "Peer " << item->ip << ":" << item->port << " misbehaved" << std::endl;
                    item->stream=nullptr;
                }
            }
        }
        for(auto idx = this->connections.begin(); idx != this->connections.end(); idx++)
        {
            auto item = *idx;
            bool destroy = true;
            if(item)
            {
                destroy = !item->stream;
            }
            if(destroy)
            {
                this->connections.erase(idx);
                idx--;
            }

        }
    }
    size_t ActiveTorrent::pieceSize(size_t piece)
    {
        if(piece == this->has.size()-1)
        {
            int64_t remainder = this->torrentSize % this->file.info.piece_length;
            if(remainder != 0)
                return (size_t)remainder;
        }
        return this->file.info.piece_length;
    }
    bool ActiveTorrent::processMessages(std::shared_ptr<TorrentPeer> peer)
    {
        if(!peer->stream) return false;
        using namespace Tesses::Framework::Serialization;
        while(peer->messages.size() >= 4)
        {
            auto len = BitConverter::ToUint32BE(peer->messages[0]);
            if(len + 4 > peer->messages.size()) break;

            if(len > 0)
            {
                uint8_t id = peer->messages[4];
                switch(id)
                {
                    case 0:
                        peer->isChokingMe=true;
                        std::cout << peer->ip << ":" << peer->port << " choked me :(" << std::endl;
                        break;
                    case 1:
                        peer->isChokingMe=false;

                        std::cout << peer->ip << ":" << peer->port << " unchoked me :)" << std::endl;
                        if(this->has.allone())
                        {
                            //send not intrested
                            std::vector<uint8_t> msg((size_t)5);
                            BitConverter::FromUint32BE(msg[0],1);
                            msg[4] = 3;

                            peer->stream->WriteBlock(msg.data(),msg.size());
                        }
                        else {
                            std::vector<uint8_t> msg((size_t)5);
                            BitConverter::FromUint32BE(msg[0],1);
                            msg[4] = 2;

                            peer->stream->WriteBlock(msg.data(),msg.size());
                        }
                        break;
                    case 2:
                        
                        std::cout << peer->ip << ":" << peer->port << " is intrested" << std::endl;
                        peer->intrested=true;
                        break;
                    case 3:
                        
                        std::cout << peer->ip << ":" << peer->port << " is not intrested" << std::endl;
                        peer->intrested=false;
                        break;
                    case 4:
                    {

                        auto pieceIndex = BitConverter::ToUint32BE(peer->messages[5]);
                        if(pieceIndex < peer->has.size())
                            peer->has.set((size_t)pieceIndex,true);
                        else return false;

                        std::cout << peer->ip << ":" << peer->port << " has piece " << pieceIndex << std::endl;
                    }
                        break;
                    case 5:
                    {
                        if(len-1 == peer->has.data().size())
                        {
                            std::copy(peer->messages.begin()+5,peer->messages.begin()+4+len,peer->has.data().begin());
                        }
                        else return false;
                    }
                        break;
                    case 6:
                    {
                        if(peer->isChoked) break;
                        if(len == 13)
                        {
                            auto pieceIndex = BitConverter::ToUint32BE(peer->messages[5]);
                            auto offset = BitConverter::ToUint32BE(peer->messages[9]);
                            auto length = BitConverter::ToUint32BE(peer->messages[13]);
                            

                            
                            std::cout << peer->ip << ":" << peer->port << " wants piece: " << pieceIndex << " offset: " << offset << " length: " << length  << std::endl;
                            


                            if(pieceIndex < this->has.size())
                            {
                                size_t _pieceSize = pieceSize((size_t)pieceIndex);
                                if((size_t)offset >= _pieceSize) return false;
                                if(length > 16384) return false;
                                if(length+offset > _pieceSize) return false;

                                if(this->has.get((size_t)pieceIndex))
                                {
                                    for(auto index = peer->cancel_requests.begin(); index != peer->cancel_requests.end(); index++)
                                    {
                                        if(index->piece == pieceIndex && index->begin == offset && index->length == length)
                                        {
                                            peer->cancel_requests.erase(index);
                                            return true;
                                        }
                                    }
                                    
                                    std::vector<uint8_t> message;
                                    message.resize(13+length);
                                    BitConverter::FromUint32BE(message[0],9+length);
                                    message[4] = 7;
                                    BitConverter::FromUint32BE(message[5], pieceIndex);
                                    BitConverter::FromUint32BE(message[9], offset);
                                    
                                    this->torrent_disk->ReadBlockAt(pieceIndex * this->file.info.piece_length + offset, message.data()+13 ,message.size()-13);
                                    peer->stream->WriteBlock(message.data(),message.size());
                                }
                            }
                            else return false;
                        } else return false;
                    }
                        break;
                    case 7:
                    {
                        if(len > 9)
                        {
                            auto pieceIndex = BitConverter::ToUint32BE(peer->messages[5]);
                            auto offset = BitConverter::ToUint32BE(peer->messages[9]);
                            auto length = len - 9;

                            std::cout << peer->ip << ":" << peer->port << " gave me a piece! piece: " << pieceIndex << " offset: " << offset << " length: " << length  << std::endl;
                            
                            if(pieceIndex < this->has.size())
                            {
                                size_t _pieceSize = pieceSize((size_t)pieceIndex);
                                if((size_t)offset >= _pieceSize) return false;
                                if(length > 16384) return false;
                                if(length+offset > _pieceSize) return false;
                                if(!this->has.get((size_t)pieceIndex))
                                {
                                    this->torrent_disk->WriteBlockAt(pieceIndex * this->file.info.piece_length + offset, peer->messages.data()+9 ,length);
                                    if(!this->blocksAquired[pieceIndex] )
                                    {
                                        auto blockCount = _pieceSize / 16384;
                                        if((_pieceSize % 16384) != 0) blockCount++;

                                        this->blocksAquired[pieceIndex] = Tesses::Framework::BitTorrent::TorrentBitField(blockCount); 
                                    }

                                    this->blocksAquired[pieceIndex]->set(offset/16384, true);
                                    if(this->blocksAquired[pieceIndex]->allone())
                                    {
                                        std::vector<uint8_t> data(_pieceSize);
                                        if(this->torrent_disk->ReadBlockAt(pieceIndex*this->file.info.piece_length,data.data(),data.size()))
                                        {
                                            auto hash = Tesses::Framework::Crypto::Sha1::ComputeHash(data.data(),data.size());
                                            if(std::equal(hash.begin(),hash.end(),this->file.info.pieces.data.begin()+(pieceIndex*20)))
                                            {
                                                this->has.set(pieceIndex,true);
                                                auto pieceFile = directory / this->file.info.name + ".tftpart";
                                                {
                                                    auto pfs = vfs->OpenFile(pieceFile,"wb");
                                                    pfs->WriteBlock(has.data().data(),has.data().size());
                                                }
                                                this->blocksAquired[pieceIndex] = std::nullopt;
                                                peer->blocksRequested[pieceIndex] = std::nullopt;
                                                {
                                                    std::vector<uint8_t> msg((size_t)9);
                                                    BitConverter::FromUint32BE(msg[0],5);
                                                    msg[4] = 4;
                                                    BitConverter::FromUint32BE(msg[5],(uint32_t)pieceIndex);
                                                    for(auto& peer2 : this->connections)
                                                    {
                                                        if(peer2->stream)
                                                        {
                                                            peer2->stream->WriteBlock(msg.data(),msg.size());
                                                        }
                                                    }
                                                }
                                                if(this->has.allone())
                                                {
                                                    TF_LOG("The torrent: " + (std::string)this->file.info.name + " is completed");
                                                    std::vector<uint8_t> msg((size_t)5);
                                                    BitConverter::FromUint32BE(msg[0],1);
                                                    msg[4] = 3;
                                                    for(auto& peer2 : this->connections)
                                                    {
                                                        if(peer2->stream)
                                                        {
                                                            //send not intrested
                                                            peer2->stream->WriteBlock(msg.data(),msg.size());
                                                        }
                                                    }
                                                }
                                                
                                            }
                                            else {
                                                //CORRUPT
                                                this->blocksAquired[pieceIndex]->zero();
                                                return false;
                                            }
                                        } 
                                        else
                                        {
                                            //CORRUPT
                                            this->blocksAquired[pieceIndex]->zero();
                                            return false;
                                        }

                                    }
                                }
                            }
                        }
                    }
                        break;
                    case 8:
                    {
                        if(len == 13)
                        {
                            auto pieceIndex = BitConverter::ToUint32BE(peer->messages[5]);
                            auto offset = BitConverter::ToUint32BE(peer->messages[9]);
                            auto length = BitConverter::ToUint32BE(peer->messages[13]);
                            CancelRequest r;
                            r.piece = pieceIndex;
                            r.begin = offset;
                            r.length = length;

                            peer->cancel_requests.push_back(r);
                        }
                    }
                        break;
                }
            }

            peer->messages.erase(peer->messages.begin(),peer->messages.begin()+4+len);
        }
        return true;
    }

    ActiveTorrent::ActiveTorrent(TorrentFile file, std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs,Tesses::Framework::Filesystem::VFSPath directory, std::string peer_id)
    {
        this->file = file;
        this->vfs = vfs;
        this->directory = directory;
        this->peer_id = peer_id;
        this->has.resize(this->file.info.pieces.data.size()/20);
        this->torrentSize = this->file.info.GetTorrentFileSize();
        this->info_hash = this->file.info.GetInfoHash();
        this->lastTime = 0;
        this->blocksAquired.resize(this->has.size());
        this->uploaded = 0;
        this->downloaded = 0;

        auto pieceFile = directory / this->file.info.name + ".tftpart";
        if(vfs->FileExists(pieceFile))
        {
            auto strm = vfs->OpenFile(pieceFile,"rb");
            strm->ReadBlock(has.data().data(),has.data().size());
        }
        this->torrent_disk = this->file.info.GetStreamFromFilesystem(vfs,directory);
    }
    std::string lookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz";

    std::string GeneratePeerId()
    {
        std::string peerId((size_t)20,' ');
        peerId.insert(0,"TFWT");
        Random r;
        for(size_t i = 4; i < peerId.size(); i++)
            peerId[i] = lookup[(int)r.Next((uint32_t)lookup.size())];
        return peerId;
    }
}