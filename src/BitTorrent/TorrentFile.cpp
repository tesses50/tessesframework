#include "TessesFramework/BitTorrent/TorrentFile.hpp"
#include "TessesFramework/Streams/MemoryStream.hpp"
#include "TessesFramework/Crypto/Crypto.hpp"
#include "TessesFramework/Http/HttpUtils.hpp"
namespace Tesses::Framework::BitTorrent
{
    
    TorrentFileInfo::TorrentFileInfo()
    {
        this->fileListOrLength = 0;
    }
    TorrentFileInfo::TorrentFileInfo(const Serialization::Bencode::BeDictionary& tkn)
    {
        Load(tkn);
    }
    int64_t TorrentFileInfo::GetTorrentFileSize()
    {
        if(std::holds_alternative<int64_t>(this->fileListOrLength))
            return std::get<int64_t>(fileListOrLength);
        else if(std::holds_alternative<std::vector<TorrentFileEntry>>(this->fileListOrLength)) {
            auto& files= std::get<std::vector<TorrentFileEntry>>(this->fileListOrLength);
            int64_t no=0;
            for(auto itm : files)
            {
                no += itm.length;
            }
            return no;
        }
        return 0;
    }
    Serialization::Bencode::BeDictionary& TorrentFileInfo::GenerateInfoDictionary()
    {
        this->info = {};
        this->info.tokens.emplace_back("name",this->name);
        this->info.tokens.emplace_back("piece length",this->piece_length);
        this->info.tokens.emplace_back("pieces",this->pieces);
        if(this->isPrivate)
            this->info.tokens.emplace_back("private",1);

        if(std::holds_alternative<int64_t>(this->fileListOrLength))
        {
            this->info.tokens.emplace_back("length",std::get<int64_t>(this->fileListOrLength));
        }
        else if(std::holds_alternative<std::vector<TorrentFileEntry>>(this->fileListOrLength))
        {
            Serialization::Bencode::BeArray a;
            for(auto& item : std::get<std::vector<TorrentFileEntry>>(this->fileListOrLength))
            {
                Serialization::Bencode::BeDictionary dict;
                dict.tokens.emplace_back("length",item.length);
                Serialization::Bencode::BeArray path;
                for(auto& p : item.path.path)
                    path.tokens.push_back(p);
                dict.tokens.emplace_back("path",path);

                a.tokens.push_back(dict);
            }
            this->info.tokens.emplace_back("files", a);
        }
        return this->info;
    }
    Serialization::Bencode::BeDictionary& TorrentFileInfo::GetInfoDictionary()
    {
        return this->info;
    }

    void TorrentFileInfo::Load(const Serialization::Bencode::BeDictionary& dict)
    {
        this->info = dict;
        auto o=dict.GetValue("name");
        if(std::holds_alternative<Serialization::Bencode::BeString>(o)) 
            this->name = std::get<Serialization::Bencode::BeString>(o);
        else
            this->name = {};
        
        o=dict.GetValue("piece length");
        if(std::holds_alternative<int64_t>(o)) 
            this->piece_length = std::get<int64_t>(o);
        else
            this->piece_length = DEFAULT_PIECE_LENGTH;
        
        o=dict.GetValue("private");
        if(std::holds_alternative<int64_t>(o)) 
            this->isPrivate = std::get<int64_t>(o);
        else
            this->isPrivate = 0;
        
        o=dict.GetValue("pieces");
        if(std::holds_alternative<Serialization::Bencode::BeString>(o)) 
            this->pieces = std::get<Serialization::Bencode::BeString>(o);
        else
            this->pieces = {};
        
        
        
        o=dict.GetValue("files");
        if(std::holds_alternative<Serialization::Bencode::BeArray>(o))
        {
             std::vector<TorrentFileEntry> ents;

            for(auto& item : std::get<Serialization::Bencode::BeArray>(o).tokens)
            {
                if(std::holds_alternative<Serialization::Bencode::BeDictionary>(item))
                {
                    TorrentFileEntry fe;
                    fe.path.relative=true;
                
                    auto& d2=std::get<Serialization::Bencode::BeDictionary>(item);
                    auto o2=d2.GetValue("length");
                    if(std::holds_alternative<int64_t>(o2))
                    {
                        fe.length = std::get<int64_t>(o2);
                    }
                    o2=d2.GetValue("path");
                    if(std::holds_alternative<Serialization::Bencode::BeArray>(o2))
                    {
                        auto& arr=std::get<Serialization::Bencode::BeArray>(o2);
                        for(auto& itm : arr.tokens)
                        {
                            if(std::holds_alternative<Serialization::Bencode::BeString>(itm))
                            {
                                fe.path.path.push_back(std::get<Serialization::Bencode::BeString>(itm));
                            }
                        }
                    }
                    ents.push_back(fe);
                }
            }
            this->fileListOrLength = ents;
        }
        else {
            o = dict.GetValue("length");
            if(std::holds_alternative<int64_t>(o))
            {
                this->fileListOrLength = std::get<int64_t>(o);
            }
            else {
                this->fileListOrLength = (int64_t)0;
            }
        }

    }
    Serialization::Bencode::BeString TorrentFileInfo::GetInfoHash()
    {
        auto strm = std::make_shared<Streams::MemoryStream>(true);
        Serialization::Bencode::Bencode::Save(strm,this->info);
        strm->Seek(0L,Streams::SeekOrigin::Begin);
        return Crypto::Sha1::ComputeHash(strm);
    }


    TorrentFile::TorrentFile()
    {
        this->created_by = "TessesFrameworkTorrent";
        this->creation_date = (int64_t)time(NULL);
    }
    TorrentFile::TorrentFile(const Serialization::Bencode::BeDictionary& tkn)
    {
        auto o=tkn.GetValue("info");
        if(std::holds_alternative<Serialization::Bencode::BeDictionary>(o))
        {
            this->info.Load(std::get<Serialization::Bencode::BeDictionary>(o));
        }
        o=tkn.GetValue("announce");
        if(std::holds_alternative<Serialization::Bencode::BeString>(o))
        {
            this->announce = std::get<Serialization::Bencode::BeString>(o);
        }
        o=tkn.GetValue("announce-list");
        if(std::holds_alternative<Serialization::Bencode::BeArray>(o))
        {
            auto& ls = std::get<Serialization::Bencode::BeArray>(o);
            for(auto& item : ls.tokens)
            {
                if(std::holds_alternative<Serialization::Bencode::BeArray>(item))
                {
                    auto ls2 = std::get<Serialization::Bencode::BeArray>(item);
                    auto item2=ls2.tokens.at(0);
                    if(std::holds_alternative<Serialization::Bencode::BeString>(item2))
                    {
                        this->announce_list.push_back(std::get<Serialization::Bencode::BeString>(item2));
                    }
                }
            }
        }
        o=tkn.GetValue("creation date");
        if(std::holds_alternative<int64_t>(o))
        {
            this->creation_date = std::get<int64_t>(o);
        }
        o=tkn.GetValue("comment");
        if(std::holds_alternative<Serialization::Bencode::BeString>(o))
        {
            this->comment = std::get<Serialization::Bencode::BeString>(o);
        }
        o=tkn.GetValue("created by");
        if(std::holds_alternative<Serialization::Bencode::BeString>(o))
        {
            this->created_by = std::get<Serialization::Bencode::BeString>(o);
        }
        o=tkn.GetValue("url-list");
        if(std::holds_alternative<Serialization::Bencode::BeArray>(o))
        {
            auto& li =std::get<Serialization::Bencode::BeArray>(o);
            for(auto& itm : li.tokens)
            {
                if(std::holds_alternative<Serialization::Bencode::BeString>(itm))
                    this->url_list.push_back(std::get<Serialization::Bencode::BeString>(itm));
            }
        }
    }

    Serialization::Bencode::BeDictionary TorrentFile::ToDictionary()
    {
        Serialization::Bencode::BeDictionary dict;
        dict.SetValue("info",this->info.GetInfoDictionary());
        dict.SetValue("announce",this->announce);
        if(!this->announce_list.empty())
        {
            Serialization::Bencode::BeArray a;
            for(auto& item : this->announce_list)
            {
                Serialization::Bencode::BeArray a2;
                a2.tokens.push_back(item);
                a.tokens.push_back(a2);
            }
            dict.SetValue("announce-list",a);
        }
        if(!this->url_list.empty())
        {
            Serialization::Bencode::BeArray ls;
            for(auto& itm : this->url_list)
            {
                ls.tokens.push_back(itm);
            }
            dict.SetValue("url-list",ls);
        }

        dict.SetValue("created by", this->created_by);
        dict.SetValue("creation date", this->creation_date);
        dict.SetValue("comment",this->comment);
        return dict;
    }

    void TorrentFile::Print(std::shared_ptr<TextStreams::TextWriter> writer)
    {
        writer->WriteLine("Announce: " + (std::string)this->announce); 
        writer->WriteLine("Announce List:");
        for(auto& item : this->announce_list)
        {
            writer->WriteLine("\t" + (std::string)item);
        }
        writer->WriteLine("Comment: " + (std::string)this->comment); 
        writer->WriteLine("Created By: " + (std::string)this->created_by); 
        Date::DateTime dt(this->creation_date);
        dt.SetToLocal();
        writer->WriteLine("Creation Date: " + dt.ToString()); 
        writer->WriteLine("Info Hash: " + Http::HttpUtils::BytesToHex(this->info.GetInfoHash().data));
        writer->WriteLine("Info:");
        writer->WriteLine("\tName: " + (std::string)this->info.name);
        writer->WriteLine(this->info.isPrivate ? "\tPrivate: true" : "\tPrivate: false");
        writer->WriteLine("\tPiece Length: " + TF_FileSize(this->info.piece_length));
        if(std::holds_alternative<int64_t>(this->info.fileListOrLength))
        {
            writer->WriteLine("\tIs Single File: true");
            writer->WriteLine("\tFile length: " + TF_FileSize((uint64_t)std::get<int64_t>(this->info.fileListOrLength)));
        }
        else if(std::holds_alternative<std::vector<TorrentFileEntry>>(this->info.fileListOrLength))
        {
            writer->WriteLine("\tIs Single File: false");
            writer->WriteLine("\tFiles:");
            auto& files = std::get<std::vector<TorrentFileEntry>>(this->info.fileListOrLength);
            for(auto& file : files)
            {
                writer->WriteLine("\t\tPath: " + file.path.ToString());
                writer->WriteLine("\t\tLength: " + TF_FileSize(file.length));
                writer->WriteLine();
            }
        }
    }
    
    std::shared_ptr<ReadWriteAt> TorrentFileInfo::GetStreamFromFilesystem(std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, const Tesses::Framework::Filesystem::VFSPath& path)
    {
        if(std::holds_alternative<std::vector<TorrentFileEntry>>(this->fileListOrLength))
        {
            return std::make_shared<TorrentDirectoryStream>(vfs,path / this->name, std::get<std::vector<TorrentFileEntry>>(this->fileListOrLength));
        }
        else if(std::holds_alternative<int64_t>(this->fileListOrLength)) {
            return std::make_shared<TorrentFileStream>(vfs,path / this->name, (uint64_t)std::get<int64_t>(this->fileListOrLength));
        }
        return nullptr;

    }

    static void ParsePieces(TorrentFileInfo* fi,std::shared_ptr<ReadWriteAt> rwa, int64_t flength)
    {   
        int64_t pieces = fi->pieces.data.size()/20;
        std::vector<uint8_t> buffer;
        buffer.resize((size_t)fi->piece_length);
        size_t lastPieceSize = (size_t)(flength % (int64_t)buffer.size());
        for(int64_t i = 0; i < pieces; i++)
        {
            size_t len = buffer.size();
            if(i == pieces-1 && lastPieceSize != 0) len = std::min(len,lastPieceSize);
            rwa->ReadBlockAt(i*buffer.size(), buffer.data(), len);
            auto hsh=Crypto::Sha1::ComputeHash(buffer.data(),len);
            std::copy(hsh.begin(),hsh.end(),fi->pieces.data.begin()+(i*20));
        }
    }
    void TorrentFileInfo::CreateFromFilesystem(std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, const Tesses::Framework::Filesystem::VFSPath& path, bool isPrivate, int64_t pieceLength)
    {
        this->isPrivate=isPrivate;
        this->piece_length = pieceLength;
        this->pieces.data.clear();
        this->name = path.GetFileName();
        int64_t len=0;
        if(vfs->FileExists(path))
        {
            auto strm = vfs->OpenFile(path,"rb");
            len = strm->GetLength();
            this->fileListOrLength= len;
            int64_t pieces = len / piece_length;
            if((len % piece_length) != 0) pieces++;

            this->pieces.data.resize(pieces*20);
        }
        else if(vfs->DirectoryExists(path))
        {
            std::vector<TorrentFileEntry> ents;
            std::function<void(Tesses::Framework::Filesystem::VFSPath,Tesses::Framework::Filesystem::VFSPath)> crawl;
            crawl= [&](Tesses::Framework::Filesystem::VFSPath inFS, Tesses::Framework::Filesystem::VFSPath inTorrent)->void{
                for(auto ent : vfs->EnumeratePaths(inFS))
                {
                    if(vfs->FileExists(ent))
                    {
                        auto strm = vfs->OpenFile(ent,"rb");
                        auto flen = strm->GetLength();
                        ents.emplace_back(inTorrent / ent.GetFileName(),flen);

                        len += flen;
                    } 
                    else if(vfs->DirectoryExists(ent))
                    {
                        crawl(ent,inTorrent / ent.GetFileName());
                    }
                }
            };
            Tesses::Framework::Filesystem::VFSPath p2;
            p2.relative=true;
            crawl(path, p2);

            this->fileListOrLength = ents;
            int64_t pieces = len / piece_length;
            if((len % piece_length) != 0) pieces++;

            this->pieces.data.resize(pieces*20);
        }
        else {
            throw std::runtime_error("File or directory does not exist");
        }
        ParsePieces(this,this->GetStreamFromFilesystem(vfs,path.GetParent()),len);


        this->GenerateInfoDictionary();
    }

    void TorrentFile::CreateTorrent(std::shared_ptr<Tesses::Framework::Streams::Stream> torrent_file_stream,const std::vector<Serialization::Bencode::BeString>& trackers,const std::vector<Serialization::Bencode::BeString>& webseeds, std::shared_ptr<Tesses::Framework::Filesystem::VFS> vfs, const Tesses::Framework::Filesystem::VFSPath& path, bool isPrivate, int64_t pieceLength, Serialization::Bencode::BeString comment, Serialization::Bencode::BeString created_by)
    {
        TorrentFile file;
        file.announce = trackers.at(0);
        file.announce_list = trackers;
        file.url_list = webseeds;
        file.comment = comment;
        file.created_by = created_by;
        file.info.CreateFromFilesystem(vfs,path,isPrivate,pieceLength);
        auto dict=file.ToDictionary();
        Serialization::Bencode::Bencode::Save(torrent_file_stream,dict);
    }
}